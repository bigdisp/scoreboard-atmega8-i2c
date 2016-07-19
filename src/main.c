/*
 * main.c
 *
 *  Created on: 10.10.2013
 *      Author: martin
 */
#include <inttypes.h>
#include "main.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "pwm.h"
#include "adc.h"
#include <util/delay.h>

#define I2C_DATA_SIZE 4

int main()
{

	// Blink output:
	DDRB |= (1 << PORTB1) | (1 << PORTB2);
	PORTB |= (1 << PB1) | (1 << PB2);
	_delay_ms(1000);
	PORTB &= ~((1 << PB1) | (1 << PB2));
	_delay_ms(100);

	// PWM
	pwm_init(); 
	pwm_enable(1);

	// ADC
	adc_init();

	// Enable Interrupts
	//sei();

	uint16_t pwm_val = 10;
	uint16_t counter = 0;

	uint8_t adc_val = 0;
	//MAIN LOOP
	while(1)
	{
		//counter++;
		// Sample use of pwm timing change for all 16 bit timers
		
		if (counter >= 10000)
		{
			pwm_val += 10;
			pwm_set_timing_16(pwm_val);
			counter = 0;
		}
		
		// Sample measurement with ADC:
		adc_set_channel(0);
		adc_val = adc_read();

		// Write adc value to pwm timing:
		pwm_set_timing_r(adc_val);



	}

	//Wird nicht erreicht
	return 0;
}
