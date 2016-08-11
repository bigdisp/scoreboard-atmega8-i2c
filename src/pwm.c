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

#define POR_PWM_0R PORTD
#define POR_PWM_1R PORTD
#define POR_PWM_0G PORTB
#define POR_PWM_1G PORTB
#define POR_PWM_0B PORTB
#define POR_PWM_1B PORTD

#define PIN_PWM_0R PIND
#define PIN_PWM_1R PIND
#define PIN_PWM_0G PINB
#define PIN_PWM_1G PINB
#define PIN_PWM_0B PINB
#define PIN_PWM_1B PIND


#define P_PWM_0R (1 << PD6)
#define P_PWM_1R (1 << PD5)
#define P_PWM_0G (1 << PB1)
#define P_PWM_1G (1 << PB2)
#define P_PWM_0B (1 << PB3)
#define P_PWM_1B (1 << PD3)

#define DDR_PWM_0R DDRD
#define DDR_PWM_1R DDRD
#define DDR_PWM_0G DDRB
#define DDR_PWM_1G DDRB
#define DDR_PWM_0B DDRB
#define DDR_PWM_1B DDRD

uint8_t enabled;

void pwm_init()
{
	pwm_set_timing_16(250);
	pwm_set_timing_8(125);

	// Port Direction (including Blue 1):
	//DDRD |= (1 << PORTD6) | (1 << PORTD5) | (1 << PORTD3);
	//DDRB |= (1 << PORTB1) | (1 << PORTB2) | (1 << PORTB3);
	DDR_PWM_0R |= P_PWM_0R;
	DDR_PWM_1R |= P_PWM_1R;
	DDR_PWM_0G |= P_PWM_0G;
	DDR_PWM_1G |= P_PWM_1G;
	DDR_PWM_0B |= P_PWM_0B;
	DDR_PWM_1B |= P_PWM_1B;
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

		enabled = 1;

		// 8 bit stuff: Red 0/1, Port D
		// Timer 0:
		// Fast PWM Mode (3), full system clock speed
		TCCR0A = (1 << WGM01) | (1 << WGM00) | (1 << COM0A1) | (1 << COM0B1);
		//TCCR1B = (1 << WGM02) | (1);
		TCCR0B = (1);
		//TIMSK0 |= (1 << OCIE0A);


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
		//TIMSK1 &= ~(1 << OCIE1A);
		//TIMSK1 &= ~(1 << OCIE1B);

		// 8 bit
		//TIMSK0 &= ~(1 << OCIE0A);
		TCCR0A = 0;
		TCCR0B = 0;
		TCCR2A = 0;
		TCCR2B = 0;
		enabled = 0;

		// Enable all outputs:
		POR_PWM_0R |= P_PWM_0R;
		POR_PWM_1R |= P_PWM_1R;
		POR_PWM_0G |= P_PWM_0G;
		POR_PWM_1G |= P_PWM_1G;
		POR_PWM_0B |= P_PWM_0B;
		POR_PWM_1B |= P_PWM_1B;
	}
}

void pwm_set_output_disabled(uint8_t rgb)
{
	if(rgb & (1 << 5))
	{
		POR_PWM_0R |= P_PWM_0R;
	}
	else
	{
		POR_PWM_0R &= ~P_PWM_0R;
	}

	if(rgb & (1 << 4))
	{
		POR_PWM_1R |= P_PWM_1R;
	}
	else
	{
		POR_PWM_1R &= ~P_PWM_1R;
	}

	if(rgb & (1 << 3))
	{
		POR_PWM_0G |= P_PWM_0G;
	}
	else
	{
		POR_PWM_0G &= ~P_PWM_0G;
	}

	if(rgb & (1 << 2))
	{
		POR_PWM_1G |= P_PWM_1G;
	}
	else
	{
		POR_PWM_1G &= ~P_PWM_1G;
	}

	if(rgb & (1 << 1))
	{
		POR_PWM_0B |= P_PWM_0B;
	}
	else
	{
		POR_PWM_0B &= ~P_PWM_0B;
	}

	if(rgb & (1 << 0))
	{
		POR_PWM_1B |= P_PWM_1B;
	}
	else
	{
		POR_PWM_1B &= ~P_PWM_1B;
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
