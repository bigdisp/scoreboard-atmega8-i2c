/*
 * interrupt.c
 *
 * Die Interrupt service Routines (ISR)
 *
 *  Created on: 26.10.2013
 *  Author: Martin Beckmann
 *
 *  TODO:
 *  - Read after Write fails for TWI
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <util/twi.h>
#include "i2c.h"
#include "pwm.h"

// I2C Interrupt
extern uint8_t i2c_tx_buf[];
extern uint8_t i2c_rx_buf[];
extern uint8_t i2c_msg_size;
extern uint8_t i2c_status;
extern u_i2c_status i2c_status_register;

ISR(TWI_vect)
{
	static uint8_t i2c_tx_buf_ptr;
	static uint8_t i2c_rx_buf_ptr;

	switch(TWSR)
	{
	// Slave Transmit Mode
	case TW_ST_SLA_ACK: 	//Transmit initialized + First byte
		i2c_tx_buf_ptr = 0;
		//no break;
	case TW_ST_DATA_ACK:	//Master demands more data (ACK'ed)
		//TODO: Muss hier die Reihenfolge gedreht werden?
		TWDR = i2c_tx_buf[i2c_tx_buf_ptr++];
		if (i2c_tx_buf_ptr >= i2c_msg_size - 1)
		{
			// Do not set TWEA because we cannot send any more data than one more byte
			TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT);
		}
		else
		{
			//Active mode:
			TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
		}
		break;

	case TW_ST_DATA_NACK:	//Master answered with NACK on last transmit
		if(i2c_tx_buf_ptr == i2c_msg_size)
		{
			i2c_status_register.last_txrx_ok = 1;
			i2c_msg_size = 0;
		}
		else
		{
			i2c_status = TWSR;
		}

		TWCR = (1 << TWEN) | (1 << TWINT); //Switch to passive mode
		break;

	// Slave Receive Mode
	case TW_SR_GCALL_ACK: //General Call
		i2c_status_register.gen_call_rx = 1;
		//no break;
	case TW_SR_SLA_ACK: //Start Condition
		i2c_status_register.rx_data_in_buf = 1;
		i2c_rx_buf_ptr = 0;
		//Active Mode, Acknowledge the next received byte
		TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
		break;

	case TW_SR_DATA_ACK: 		//Data Received, ACK'ed
	case TW_SR_GCALL_DATA_ACK: 	//Same for general call
		i2c_rx_buf[i2c_rx_buf_ptr++] = TWDR;
		i2c_status_register.last_txrx_ok = 1;
		if(i2c_rx_buf_ptr >= I2C_BUFFER_SIZE - 1)
		{
			//Do not set TWEA because we cannot receive any more data than one more byte
			TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT);
		}
		else
		{
			//Active Mode, Acknowledge the next received byte
			TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
		}
		break;

	// Stop / Repeated Start:
	case TW_SR_STOP:
		if (i2c_status_register.rx_data_in_buf == 1 && i2c_rx_buf[0] == 0x10 && i2c_msg_size > 0)
		{
			// We have received something and are ready to transmit something
			// Maybe this doesn't make sense because we didn't react
			// on the address received, but keep active
			TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
		}
		else
		{
			TWCR = (1 << TWEN) | (1 << TWINT); // Switch to passive mode
		}
		break;

	// All conditions that disable rx/tx:
	case TW_SR_DATA_NACK:
	case TW_SR_GCALL_DATA_NACK:
		i2c_rx_buf[i2c_rx_buf_ptr++] = TWDR;

		//no break
	case TW_ST_LAST_DATA:
	case TW_BUS_ERROR:
	default:
		i2c_status = TWSR;
		TWCR = (1 << TWEN) | (1 << TWINT); // Switch to passive mode
		break;
	}
}

// PWM Interrupts
#if PWM_ENABLED
volatile extern uint16_t pwm_t_on;
volatile extern uint16_t pwm_t_off;
volatile extern uint8_t pwm_mask;
#if PWM_PORT2_ACTIVE
volatile extern uint8_t pwm_mask2;
#endif
ISR(TIMER1_COMPA_vect)
{
	static uint8_t outputs_enabled = 0;
	if(outputs_enabled)
	{
		outputs_enabled = 0;
		//TODO: Achtung: Das sind 16 bit Werte, also prinzipiell 2 Takte -> Interrupt disable?
		OCR1A += pwm_t_off;
		PWM_PORT1 = 0;
#if PWM_PORT2_ACTIVE
		PWM_PORT2 = 0;
#endif
	}
	else
	{
		outputs_enabled = 1;
		OCR1A += pwm_t_on;
		PWM_PORT1 = pwm_mask;
#if PWM_PORT2_ACTIVE
		PWM_PORT2 = pwm_mask2;
#endif
	}
}

#endif
