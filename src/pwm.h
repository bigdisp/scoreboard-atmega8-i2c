/*
 * pwm.h
 *
 *  Created on: 01.03.2014
 *      Author: martin
 */

#ifndef PWM_H_
#define PWM_H_

// This can be used to completely disable PWM at compile time
#define PWM_ENABLED 1

// All compiles should already fail, if they require PWM and it is disabled, so remove all other header content
#if PWM_ENABLED
// Port definitions
#define PWM_PORT1 PORTB
#define PWM_PORT2 PORTC
// Do we even want to use a second output port?
#define PWM_PORT2_ACTIVE 0


void pwm_update_port1(uint8_t mask);
#if PWM_PORT2_ACTIVE
void pwm_update_port2(uint8_t mask);
#endif
/**
 * Set the timing of on/off cycles in terms of clock cycles (16 bit resolution)
 * Using a zero will result in 2^16 cycles.
 */
void pwm_set_timing(uint16_t active, uint16_t disabled);
/**
 * Enable the PWM (ISR will be executed, if Interrupts are enabled, the PWM will run)
 */
void pwm_enable(uint8_t enable);
/**
 * Initializes internal state of the values, sets default display to empty. PWM is not active after this.
 */
void pwm_init(uint16_t active, uint16_t disabled);

#endif
#endif /* PWM_H_ */
