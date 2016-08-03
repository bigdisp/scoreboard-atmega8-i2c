/*
 * pwm.c
 *
 *  Created on: 01.03.2014
 *      Author: Martin Beckmann
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "pwm.h"
#if PWM_ENABLED


uint8_t enabled;

void pwm_init()
{
	pwm_set_timing_16(250);
	pwm_set_timing_8(125);
}

void pwm_enable(uint8_t enable)
{
	if(enable)
	{
		//16 bit stuff: Green 0/1
		// Fast PWM Mode, 9 bit set on bottom, clear on match, full system clock speed
		//TCCR1A = (1 << WGM11) | (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1);
		//TCCR1B = (1 << WGM13) | (1 << WGM12) | (1);
		TCCR1A = (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);
		TCCR1B = (1 << WGM12) | (1);
								// Use 2,3,4,5 for 8,64,256,1024 (higher values are for external capture prescaling)
		//TIMSK1 |= (1 << OCIE1A) | (1 << OCIE1B);		// Output compare A and B Match interrupt enable
								// Use TOIE1 for overflow interrupt enable
		//Port direction (including Blue 0):
		DDRB |= (1 << PORTB1) | (1 << PORTB2) | (1 << PORTB3);


		enabled = 1;

		// 8 bit stuff: Red 0/1, Port D
		// Timer 0:
		// Fast PWM Mode (3), full system clock speed
		TCCR0A = (1 << WGM01) | (1 << WGM00) | (1 << COM0A1) | (1 << COM0B1);
		//TCCR1B = (1 << WGM02) | (1);
		TCCR0B = (1);
		//TIMSK0 |= (1 << OCIE0A);
		// Port Direction (including Blue 1):
		DDRD |= (1 << PORTD6) | (1 << PORTD5) | (1 << PORTD3);

		// Timer 2:
		// Fast PWM Mode (3), full system clock speed
		TCCR2A = (1 << WGM01) | (1 << WGM00) | (1 << COM0A1) | (1 << COM0B1);
		TCCR2B = (1);

	}
	else
	{
		// 16 bit
		TCCR1B = 0;
		TCCR1A = 0;
		DDRB &= ~(1 << PORTB1);
		DDRB &= ~(1 << PORTB2);
		DDRB &= ~(1 << PORTB3);
		//TIMSK1 &= ~(1 << OCIE1A);
		//TIMSK1 &= ~(1 << OCIE1B);

		// 8 bit
		//TIMSK0 &= ~(1 << OCIE0A);
		TCCR0A = 0;
		TCCR0B = 0;
		TCCR2A = 0;
		TCCR2B = 0;
		enabled = 0;
		DDRD &= ~(1 << PORTD6);
		DDRD &= ~(1 << PORTD5);
		DDRD &= ~(1 << PORTD3);
	}
}


void pwm_set_timing_16(uint16_t active)
{
	// Disable interrupts for 16 bit operation
	uint8_t tmp_sreg;
	tmp_sreg = SREG;
	cli();

	OCR1A = active;
	OCR1B = active;

	SREG = tmp_sreg;
}

void pwm_set_timing_g(uint16_t active, uint8_t port)
{
	uint8_t tmp_sreg;
	tmp_sreg = SREG;
	cli();

	if (port == 0)
	{
		OCR1A = active;
	}
	else
	{
		OCR1B = active;
	}

	SREG = tmp_sreg;
}

void pwm_set_timing_b(uint8_t active, uint8_t port)
{
	if (port == 0)
	{
		OCR2A = active;
	}
	else
	{
		OCR2B = active;
	}
}

void pwm_set_timing_r(uint8_t active, uint8_t port)
{
	if (port == 0)
	{
		OCR0A = active;
	}
	else
	{
		OCR0B = active;
	}
}

void pwm_set_timing_8(uint8_t active)
{
	OCR0A = active;
	OCR0B = active;
	OCR2A = active;
	OCR2B = active;
}

#endif
