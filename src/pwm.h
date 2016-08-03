/*
 * pwm.h
 *
 *  Created on: 01.03.2014
 *  Author: Martin Beckmann
 */

#ifndef PWM_H_
#define PWM_H_

/// This can be used to completely disable PWM at compile time
#define PWM_ENABLED 1

// All compiles should already fail, if they require PWM and it is disabled, so remove all other header content
#if PWM_ENABLED

/**
 * Initializes PWM registers to about 50% duty cycle.
 */
void pwm_init();

/**
 * Enables or disables PWM ports. Currently, sets PWM 16 bit ports to 9 bit operation and selects fast PWM mode.
 * Therefore use at most 512 for all 16 bit values, everything else will equal to always on.
 * @param enable 1: enable 0: disable
 */
void pwm_enable(uint8_t enable);

/**
 * Set the timing for all 16 bit PWM ports. The active cycle is set, thus LED are brighter for higher values.
 */
void pwm_set_timing_16(uint16_t active);

/**
 * Set the timing for all 8 bit PWM ports. The active cycle is set, thus LED are brighter for higher values.
 */
void pwm_set_timing_8(uint8_t active);

/**
 * Set timing for green value (16 bit).
 */
void pwm_set_timing_g(uint16_t active, uint8_t port);

/**
 * Set timing for blue value (8 bit).
 */
void pwm_set_timing_b(uint8_t active, uint8_t port);

/**
 * Set timing for red value (8 bit).
 */
void pwm_set_timing_r(uint8_t active, uint8_t port);

#endif
#endif /* PWM_H_ */
