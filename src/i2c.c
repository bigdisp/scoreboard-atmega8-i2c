/*
 * i2c.c
 *
 *  Created on: 10.10.2013
 *      Author: martin
 */

#include<util/twi.h>
#include<avr/io.h>
#include "i2c.h"

uint8_t i2c_buf[I2C_BUFFER_SIZE];
uint8_t i2c_msg_size = 0;
uint8_t i2c_status = I2C_NO_STATE;
u_i2c_status i2c_status_register = {0};

void i2c_init(uint8_t address)
{
	TWAR = (address << 1);
	TWDR = 0xFF;
	TWCR =  (1 << TWEN);  // Enable TWI, don't answer to any calls yet

	//Empty buffer:
	for(uint8_t i = 0; i < I2C_BUFFER_SIZE; i++)
	{
		i2c_buf[i] = 0;
	}
	i2c_status = 0;
}

uint8_t i2c_get_status()
{
	return i2c_status;
}

u_i2c_status i2c_get_status_register()
{
	return i2c_status_register;
}

/**
 * Check whether the Interface is currently busy
 */
uint8_t i2c_busy()
{
	return TWCR & (1<<TWIE); //When Interrupt is enabled, the i2c Bus is busy
}

/**
 * Returns current status after waiting for the current operation to finish
 */
uint8_t i2c_wait_busy()
{
	while(i2c_busy());
	return i2c_status;
}

/**
 * Send provided data the next time this slave is addressed with a SLA+W
 *
 * Also enables RX Acknowledge
 */
void i2c_slave_start_tx_data(uint8_t * msg, uint8_t size)
{
	uint8_t tmp;

	//Wait until whatever was done before is finished
	while(i2c_busy());

	i2c_msg_size = size;

	for(tmp = 0; tmp < size; tmp++)
	{
		i2c_buf[tmp] = msg[tmp];
	}
	i2c_status_register.all = 0;
	i2c_status = I2C_NO_STATE;

	// Enable i2c, enable interrupt, clear interrupt flag
	// Respond with ACK, the next time we are called
	TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
}

/**
 * Enables RX Mode for this slave. Also enables TX of whatever is in the buffer
 */
void i2c_slave_start_rx()
{
	while(i2c_busy());

	i2c_status_register.all = 0;
	i2c_status = I2C_NO_STATE;

	// Enable i2c, enable interrupt, clear interrupt flag
	// Respond with ACK, the next time we are called
	TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
}

uint8_t i2c_get_data(uint8_t * msg, uint8_t size)
{
	uint8_t i;

	while(i2c_busy());

	if(i2c_status_register.last_txrx_ok)
	{
		for(i = 0; i < size; ++i)
		{
			msg[i] = i2c_buf[i];
		}
		i2c_status_register.rx_data_in_buf = 0;
	}
	return i2c_status_register.last_txrx_ok;
}

