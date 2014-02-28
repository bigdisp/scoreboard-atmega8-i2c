/*
 * interrupt.c
 *
 * Die Interrupt service Routines (ISR)
 *
 *  Created on: 26.10.2013
 *      Author: martin
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <util/twi.h>
#include "i2c.h"


/**
 * I2C/TWI Interrupt
 *
 * Causes a reaction to action on the TWI Interface
 */
/*extern uint8_t i2c_result;
extern uint8_t i2c_status;
ISR(TWI_vect)
{
	i2c_status = TWSR;
	//Check status:
	if(TWSR == TW_SR_SLA_ACK) //0x60
	{
		//Own SLA+W has been received, ACK was returned
		//TWCR |= (1 << TWINT) | (1 << TWEA); // ACK will be returned after receiving data
		TWCR &= ~(1 << TWEA); // NACK will be returned after receiving data
		TWCR |= (1 << TWINT);
	}
	else if(TWSR == TW_SR_DATA_NACK) //0x88
	{
		//Own SLA+W was received before, Now data arrived and was NACK'ed
		//
		i2c_result = TWDR;

		//Disable receiving for now (TWEA = 0)
		//but stay in slave mode (TWSTA = 0, TWSTO = 0)
		TWCR &= ~((1 << TWSTA) | (1 << TWSTO) | (1 << TWEA));
		TWCR |= (1 << TWINT);
	}
	else if(TWSR == TW_SR_DATA_ACK) //0x80
	{
		//Eigentlich sollte das gar nicht ausgeführt werden
		//Own SLA+W was received before, Now data arrived and was ACK'ed
		//Store the result for the display
		//The display must signal that it is ready before next ACK
		if(TWCR & (1<<TWEA))
		{
			i2c_result = TWDR;
		}
		else
		{
			//Das ist Mist
			//Protokollfehler
		}
		TWCR &= ~(1 << TWEA);
		TWCR |= (1 << TWINT);

	}
	else if(TWSR == TW_ST_SLA_ACK) //0x8A
	{
		//Own SLA+R has been received, ACK was returned
		//Now we need to send some data?
	}
}*/

extern uint8_t i2c_buf[];
extern uint8_t i2c_msg_size;
extern uint8_t i2c_status;
extern u_i2c_status i2c_status_register;

ISR(SIG_2WIRE_SERIAL)
{
	static uint8_t i2c_buf_ptr;

	switch(TWSR)
	{
	// Slave Transmit Mode
	case TW_ST_SLA_ACK: 	//Transmit initialized + First byte
		i2c_buf_ptr = 0;
		//no break;
	case TW_ST_DATA_ACK:	//Master demands more data (ACK'ed)
		TWDR = i2c_buf[i2c_buf_ptr++];
		//Active mode:
		TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
		break;

	case TW_ST_DATA_NACK:	//Master answered with NACK on last transmit
		if(i2c_buf_ptr == i2c_msg_size)
		{
			i2c_status_register.last_txrx_ok = 1;
		}
		else
		{
			i2c_status = TWSR;
		}

		TWCR = (1 << TWEN); //Switch to passive mode
		break;

	// Slave Receive Mode
	case TW_SR_GCALL_ACK: //General Call
		i2c_status_register.gen_call_rx = 1;
		//no break;
	case TW_SR_SLA_ACK: //Start Condition
		i2c_status_register.rx_data_in_buf = 1;
		i2c_buf_ptr = 0;
		//Active Mode, Acknowledge the next received byte
		TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
		break;

	case TW_SR_DATA_ACK: 		//Data Received, ACK'ed
	case TW_SR_GCALL_DATA_ACK: 	//Same for general call
		i2c_buf[i2c_buf_ptr++] = TWDR;
		i2c_status_register.last_txrx_ok = 1;
		//Active Mode, Acknowledge the next received byte
		TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
		break;

	case TW_SR_STOP:
		TWCR = (1 << TWEN); // Switch to passive mode
		break;
	case TW_SR_DATA_NACK:
	case TW_SR_GCALL_DATA_NACK:
	case TW_ST_LAST_DATA:
	case TW_BUS_ERROR:
	default:
		i2c_status = TWSR;
		TWCR = (1 << TWEN); // Switch to passive mode
		break;
	}
}
