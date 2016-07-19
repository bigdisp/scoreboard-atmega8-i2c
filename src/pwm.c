/*
 * pwm.c
 *
 *  Created on: 01.03.2014
 *      Author: martin
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
		//16 bit stuff
		// Fast PWM Mode, 9 bit set on bottom, clear on match, full system clock speed
		//TCCR1A = (1 << WGM11) | (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1);
		//TCCR1B = (1 << WGM13) | (1 << WGM12) | (1);
		TCCR1A = (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);
		TCCR1B = (1 << WGM12) | (1);
								// Use 2,3,4,5 for 8,64,256,1024 (higher values are for external capture prescaling)
		//TIMSK1 |= (1 << OCIE1A) | (1 << OCIE1B);		// Output compare A and B Match interrupt enable
								// Use TOIE1 for overflow interrupt enable
		//Port direction:
		DDRB |= (1 << PORTB1) | (1 << PORTB2);


		enabled = 1;

		// 8 bit stuff
		// Fast PWM Mode (3), full system clock speed
		TCCR0A = (1 << WGM01) | (1 << WGM00) | (1 << COM0A1) | (1 << COM0B1);
		//TCCR1B = (1 << WGM02) | (1);
		TCCR1B = (1);
		//TIMSK0 |= (1 << OCIE0A);
		DDRD |= (1 << PORTD6);
	}
	else
	{
		// 16 bit
		TCCR1B = 0;
		TCCR1A = 0;
		DDRB &= ~(1 << PORTB1);
		DDRB &= ~(1 << PORTB2);
		//TIMSK1 &= ~(1 << OCIE1A);
		//TIMSK1 &= ~(1 << OCIE1B);
		
		// 8 bit
		//TIMSK0 &= ~(1 << OCIE0A);
		TCCR0A = 0;
		TCCR0B = 0;
		enabled = 0;
		DDRD &= ~(1 << PORTD6);
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

void pwm_set_timing_g(uint16_t active)
{
	uint8_t tmp_sreg;
	tmp_sreg = SREG;
	cli();
	
	OCR1A = active;

	SREG = tmp_sreg;
}

void pwm_set_timing_b(uint16_t active)
{
	uint8_t tmp_sreg;
	tmp_sreg = SREG;
	cli();
	
	OCR1B = active;

	SREG = tmp_sreg;
}

void pwm_set_timing_r(uint8_t active)
{
	OCR0A = active;
}

void pwm_set_timing_8(uint8_t active)
{
	OCR0A = active;
}

#endif
