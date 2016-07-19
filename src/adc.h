/*
 * adc.h
 *
 *  Created on: 19.07.2016
 *      Author: Martin Beckmann
 */

#ifndef ADC_H_
#define ADC_H_

/**
 * Init adc, set channel 0, read high bit only
 */
void adc_init();

/**
 * Set the channel to read. Value should correspond to adc input pin.
 */
void adc_set_channel(uint8_t channel);

/**
 * Start a measurement asynchronously.
 */
void adc_start_measurement();

/**
 * Read a value from the adc. This call blocks execution until a measurement is finished and starts one if none was started.
 */
uint8_t adc_read();

#endif /* ADC_H_ */
