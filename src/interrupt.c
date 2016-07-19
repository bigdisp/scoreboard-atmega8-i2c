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
#include "pwm.h"



// PWM Interrupts
/*volatile extern uint8_t pwm_t_on_r;
volatile extern uint16_t pwm_t_on_g;
volatile extern uint16_t pwm_t_on_b;	
volatile extern uint16_t pwm_t_total16;	
volatile extern uint8_t pwm_t_total8;
ISR(TIMER1_COMPA_vect)
{
	static uint8_t output_enabled = 0;
	if(output_enabled)
	{
		output_enabled = 0;
		OCR1A = pwm_t_total16 - pwm_t_on_g;
	}
	else
	{
		output_enabled = 1;
		OCR1A = pwm_t_on_g;
	}
}

ISR(TIMER1_COMPB_vect)
{
	static uint8_t output_enabled = 0;
	if(output_enabled)
	{
		output_enabled = 0;
		OCR1B = pwm_t_total16 - pwm_t_on_b;
	}
	else
	{
		output_enabled = 1;
		OCR1B = pwm_t_on_b;
	}
}

ISR(TIMER0_COMPA_vect)
{
	static uint8_t output_enabled = 0;
	if(output_enabled)
	{
		output_enabled = 0;
		OCR0A = pwm_t_total8 - pwm_t_on_r;
	}
	else
	{
		output_enabled = 1;
		OCR0A = pwm_t_on_r;
	}
}*/

