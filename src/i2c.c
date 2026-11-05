/*
 * i2c.c
 *
 *  Created on: 10.10.2013
 *  Author: Martin Beckmann
 */

#include <avr/io.h>
#include <util/atomic.h>
#include <util/twi.h>
#include "i2c.h"
#include "i2c_private.h"

/// A transfer without any bus activity for this long is considered dead.
#define I2C_STALL_TIMEOUT_US 50000U

volatile i2c_message_t i2c_rx_queue[I2C_RX_QUEUE_SIZE];
volatile uint8_t i2c_rx_head = 0;
volatile uint8_t i2c_rx_tail = 0;

volatile uint8_t i2c_tx_next[I2C_BUFFER_SIZE];
volatile uint8_t i2c_tx_next_len = 0;

volatile uint8_t i2c_transfer_active = 0;
volatile uint8_t i2c_activity = 0;

/// 1 once i2c_slave_enable() was called
static uint8_t i2c_enabled = 0;

/**
 * Reset the TWI state machine and (re)arm it.
 *
 * Disabling the TWI aborts any ongoing transfer and releases SDA/SCL.
 * Must be called with interrupts disabled.
 */
static void i2c_hw_restart(void)
{
	TWCR = 0;
	i2c_transfer_active = 0;

	if (i2c_enabled)
	{
		TWCR = I2C_TWCR_ACK;
	}
	else
	{
		// Enabled, but do not acknowledge the own address yet
		TWCR = (1 << TWEN) | (1 << TWINT);
	}
}

void i2c_init(uint8_t address)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		i2c_enabled = 0;

		// Fully reset the TWI first. The hardware may still be in an
		// addressed state (e.g. when started from a bootloader), and
		// writing TWCR without TWINT would not release the bus.
		TWCR = 0;

		TWSR = 0; // Prescaler 1 (irrelevant for slave, keeps status bits clean)
		TWAR = (address << 1); // No general call
		TWDR = 0xFF;

		i2c_rx_head = 0;
		i2c_rx_tail = 0;
		i2c_tx_next_len = 0;
		for (uint8_t i = 0; i < I2C_BUFFER_SIZE; i++)
		{
			i2c_tx_next[i] = 0;
		}

		i2c_hw_restart();
	}
}

void i2c_slave_enable(void)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		i2c_enabled = 1;
		i2c_hw_restart();
	}
}

void i2c_set_address(uint8_t address)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		TWCR = 0;
		TWAR = (address << 1);
		i2c_hw_restart();
	}
}

uint8_t i2c_get_address(void)
{
	return TWAR >> 1;
}

void i2c_slave_set_tx_data(const uint8_t * msg, uint8_t size)
{
	if (size > I2C_BUFFER_SIZE)
	{
		size = I2C_BUFFER_SIZE;
	}

	// The ISR latches the data on the next SLA+R, so a read in progress is
	// not affected and always returns consistent data.
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		for (uint8_t i = 0; i < size; i++)
		{
			i2c_tx_next[i] = msg[i];
		}
		i2c_tx_next_len = size;
	}
}

uint8_t i2c_get_message(uint8_t * msg, uint8_t size)
{
	uint8_t tail = i2c_rx_tail;
	uint8_t len;
	uint8_t i;

	if (tail == i2c_rx_head)
	{
		return 0;
	}

	// The ISR never touches this slot until tail is advanced
	volatile i2c_message_t * slot = &i2c_rx_queue[tail & (I2C_RX_QUEUE_SIZE - 1)];
	len = slot->len;

	for (i = 0; i < size; i++)
	{
		msg[i] = (i < len && i < I2C_BUFFER_SIZE) ? slot->data[i] : 0x00;
	}

	i2c_rx_tail = tail + 1;

	return len;
}

void i2c_supervise(uint16_t elapsed_us)
{
	static uint8_t last_activity = 0;
	static uint16_t stalled_us = 0;

	uint8_t activity = i2c_activity;

	if (!i2c_transfer_active || activity != last_activity)
	{
		last_activity = activity;
		stalled_us = 0;
		return;
	}

	if (stalled_us < I2C_STALL_TIMEOUT_US)
	{
		stalled_us += elapsed_us;
		return;
	}

	// Master vanished in the middle of a transfer. Release the bus.
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		// Re-check, the transfer might have finished in the meantime
		if (i2c_transfer_active && activity == i2c_activity)
		{
			i2c_hw_restart();
		}
	}
	stalled_us = 0;
}
