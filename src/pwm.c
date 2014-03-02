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

volatile uint8_t pwm_mask;
#if PWM_PORT2_ACTIVE
volatile uint8_t pwm_mask2;
#endif
uint8_t enabled;
volatile uint16_t pwm_t_on;		// Fraction of 16 bit, the light should be on. Can be up to 65535
volatile uint16_t pwm_t_off;		// Fraction of 16 bit, the light should be off. Can be up to 65535

void pwm_init(uint16_t active, uint16_t disabled)
{
	pwm_t_on  = active;
	pwm_t_off = disabled;
	enabled   = 0;
	pwm_mask  = 0;
#if PWM_PORT2_ACTIVE
	pwm_mask2 = 0;
#endif
}

void pwm_enable(uint8_t enable)
{
	if(enable)
	{
		TCCR1B = 1;				// Run Timer at full system clock
								// Use 2,3,4,5 for 8,64,256,1024 (higher values are for external capture prescaling)
		TIMSK |= (1 << OCIE1A);	// Output compare A Match interrupt enable
								// Use TOIE1 for overflow interrupt enable
		enabled = 1;
	}
	else
	{
		TCCR1B = 0;
		TIMSK &= ~(1 << OCIE1A);
		enabled = 0;
		PWM_PORT1 = pwm_mask;
#if PWM_PORT2_ACTIVE
		PWM_PORT2 = pwm_mask2;
#endif
	}
}

void pwm_update_port1(uint8_t mask)
{
	pwm_mask = mask;
	if(enabled == 0)
	{
		PWM_PORT1 = mask;
	}
}

#if PWM_PORT2_ACTIVE
void pwm_update_port2(uint8_t mask)
{
	pwm_mask2 = mask;
	if(enabled == 0)
	{
		PWM_PORT2 = mask;
	}
}

#endif


void pwm_set_timing(uint16_t active, uint16_t disabled)
{
	//TODO: 16 Bit Operationen: evtl. Interrupts off?
	uint8_t tmp_sreg;
	tmp_sreg = SREG;
	cli();

	pwm_t_on  = active;
	pwm_t_off = disabled;

	SREG = tmp_sreg;
}
#endif
