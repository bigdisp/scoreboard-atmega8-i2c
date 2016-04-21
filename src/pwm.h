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
// Port definitions
#define PWM_PORT1 PORTB
#define PWM_PORT2 PORTC
#define PWM_DDR1 DDRB
#define PWM_DDR2 DDRC

/// Do we want to use a second output port?
#define PWM_PORT2_ACTIVE 0

/**
 * Set the output data for the first port.
 *
 * Must be in seven segment wiring format. See anzeigeboard.h
 * @param mask Bit mask for enabled led. Enabled led correspond to 1.
 */
void pwm_update_port1(uint8_t mask);
#if PWM_PORT2_ACTIVE
/**
 * Set the output data for the second port.
 *
 * @see pwm_update_port1
 */
void pwm_update_port2(uint8_t mask);
#endif

/**
 * Set the timing of on/off cycles in terms of clock cycles (16 bit resolution).
 *
 * Using a zero will result in 2^16 cycles.
 * @param active Number of clock cycles, led are on.
 * @param disabled Number of clock cycles, led are off.
 */
void pwm_set_timing(uint16_t active, uint16_t disabled);

/**
 * Enable the PWM (ISR will be executed, if Interrupts are enabled, the PWM will run).
 * @param enable Enable, if parameter is 1, otherwise disable.
 */
void pwm_enable(uint8_t enable);

/**
 * Initializes internal state of the values, sets default display to empty. PWM is not active after this.
 * @param active Number of clock cycles, led are on.
 * @param disabled Number of clock cycles, led are off.
 */
void pwm_init(uint16_t active, uint16_t disabled);

#endif
#endif /* PWM_H_ */
