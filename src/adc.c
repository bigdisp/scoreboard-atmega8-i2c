/*
 * adc.c
 *
 *  Created on: 19.07.2016
 *      Author: Martin Beckmann
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"

uint8_t meas_started = 0;

void adc_init()
{
	// Interne Referenzspannung AVCC, Linksbündiges Ergebnis (bei 8 bit Verwendung werden die höherwertigen Bits verwendet)
	// Kanal 0 Pin PC0
	ADMUX = (1 << REFS0) | (1 << ADLAR);

	// ADC Enable, Prescaler 64
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
}

void adc_set_channel(uint8_t channel)
{
	uint8_t tmp = ADMUX & ((1 << REFS1) | (1 << REFS0) | (1 << ADLAR));
	ADMUX = tmp | (channel & ((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0)));
}

void adc_start_measurement()
{
	// Start measurement:
	ADCSRA |= (1 << ADSC);
	meas_started = 1;
}

uint8_t adc_read()
{
	if (!meas_started)
	{
		adc_start_measurement();
	}
	meas_started = 0;

	// Wait for conversion to finish:
	while (ADCSRA & (1 << ADSC))
	{

	}

	return ADCH;
}
