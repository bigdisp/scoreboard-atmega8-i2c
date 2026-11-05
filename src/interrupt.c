/*
 * interrupt.c
 *
 * Die Interrupt service Routines (ISR)
 *
 *  Created on: 26.10.2013
 *  Author: Martin Beckmann
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <util/twi.h>
#include "i2c.h"
#include "i2c_private.h"

/**
 * TWI slave state machine.
 *
 * The slave always re-arms itself (TWEA set) at the end of a transaction,
 * so it never depends on the main loop to answer the next one. Received
 * writes are committed to the receive queue on STOP / repeated START.
 */
ISR(TWI_vect)
{
	// Transfer state, only used inside the ISR
	static uint8_t tx_buf[I2C_BUFFER_SIZE];
	static uint8_t tx_len;
	static uint8_t tx_ptr;
	static uint8_t rx_ptr;
	static uint8_t rx_discard;

	uint8_t twcr = I2C_TWCR_ACK;
	volatile i2c_message_t * slot = &i2c_rx_queue[i2c_rx_head & (I2C_RX_QUEUE_SIZE - 1)];

	++i2c_activity;

	// Mask out the prescaler bits
	switch (TW_STATUS)
	{
	// --- Slave receive mode -------------------------------------------
	case TW_SR_SLA_ACK:            // Own SLA+W received, ACK returned
	case TW_SR_GCALL_ACK:          // General call received, ACK returned
	case TW_SR_ARB_LOST_SLA_ACK:   // (master mode is not used, handle anyway)
	case TW_SR_ARB_LOST_GCALL_ACK:
		i2c_transfer_active = 1;
		rx_ptr = 0;
		// If the queue is full, refuse the data so the master sees an error
		// instead of silently losing the command.
		rx_discard = ((uint8_t)(i2c_rx_head - i2c_rx_tail) >= I2C_RX_QUEUE_SIZE);
		if (rx_discard)
		{
			twcr = I2C_TWCR_NACK;
		}
		break;

	case TW_SR_DATA_ACK:           // Data received, ACK returned
	case TW_SR_GCALL_DATA_ACK:
		if (rx_ptr < I2C_BUFFER_SIZE)
		{
			slot->data[rx_ptr++] = TWDR;
		}
		if (rx_ptr >= I2C_BUFFER_SIZE)
		{
			// Buffer is full, NACK the next byte
			twcr = I2C_TWCR_NACK;
		}
		break;

	case TW_SR_DATA_NACK:          // Data received, NACK returned (buffer full or discarding)
	case TW_SR_GCALL_DATA_NACK:
		// The slave is no longer addressed now, there won't be a STOP
		// interrupt. Commit what has been received so far.
		// no break
	case TW_SR_STOP:               // STOP or repeated START received
		if (i2c_transfer_active && !rx_discard && rx_ptr > 0)
		{
			slot->len = rx_ptr;
			++i2c_rx_head;
		}
		rx_ptr = 0;
		rx_discard = 0;
		i2c_transfer_active = 0;
		break;

	// --- Slave transmit mode ------------------------------------------
	case TW_ST_SLA_ACK:            // Own SLA+R received, ACK returned
	case TW_ST_ARB_LOST_SLA_ACK:
		i2c_transfer_active = 1;
		// Latch the data so that it cannot change during the transfer
		tx_len = i2c_tx_next_len;
		for (uint8_t i = 0; i < tx_len; i++)
		{
			tx_buf[i] = i2c_tx_next[i];
		}
		tx_ptr = 0;
		// no break
	case TW_ST_DATA_ACK:           // Data transmitted, ACK received
		if (tx_ptr < tx_len)
		{
			TWDR = tx_buf[tx_ptr++];
		}
		else
		{
			TWDR = 0xFF;
		}
		if (tx_ptr >= tx_len)
		{
			// This is the last byte we have
			twcr = I2C_TWCR_NACK;
		}
		break;

	case TW_ST_DATA_NACK:          // Data transmitted, NACK received (master is done)
	case TW_ST_LAST_DATA:          // Last byte transmitted, ACK received (master wanted more)
		i2c_transfer_active = 0;
		break;

	// --- Errors -------------------------------------------------------
	case TW_BUS_ERROR:             // Illegal START/STOP
		// Recover according to the datasheet: set TWSTO and clear TWINT.
		// No STOP is actually sent, the hardware just releases the bus.
		rx_ptr = 0;
		rx_discard = 0;
		i2c_transfer_active = 0;
		twcr = I2C_TWCR_ACK | (1 << TWSTO);
		break;

	default:
		// Unexpected state, return to the not addressed slave mode
		i2c_transfer_active = 0;
		break;
	}

	TWCR = twcr;
}
