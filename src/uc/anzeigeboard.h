/*
 * anzeigeboard.h
 *
 *  Created on: 27.04.2016
 *  Author: Martin Beckmann
 *
 *  WARNING: This file should be only included in anzeigeboard.c!
 */

#ifndef UC_ANZEIGEBOARD_H_
#define UC_ANZEIGEBOARD_H_
#ifndef __IN_ANZEIGEBOARD_C
	#error This header file should be only included from anzeigeboard.c it is intended for test abstraction
#endif

//Segment definititions
// This definition was moved to anzeigeboard.h:
//#define SEG_A  (1 << 4)
//#define SEG_B  (1 << 3)
//#define SEG_C  (1 << 1)
//#define SEG_D  (1 << 7)
//#define SEG_E  (1 << 6)
//#define SEG_F  (1 << 5)
//#define SEG_G  (1 << 2)
//#define SEG_PT (1 << 0)
#define SEG_A  (1 << 4)
#define SEG_B  (1 << 3)
#define SEG_C  (1 << 1)
#define SEG_D  (1 << 7)
#define SEG_E  (1 << 6)
#define SEG_F  (1 << 5)
#define SEG_G  (1 << 2)
#define SEG_PT (1 << 0)
/*
 *         AAAAAAA
 *        F       B
 *        F       B
 *        F       B
 *         GGGGGGG
 *        E       C
 *        E       C
 *        E       C
 *         DDDDDDD   PT
 *
 */

/// Port definitions
#if PWM_ENABLED
	#define ANZ_PORT2_ACTIVE	PWM_PORT2_ACTIVE
	#if ANZ_PORT2_ACTIVE
		#define ANZ_PORT_ZEHNER 	PWM_PORT2
		#define ANZ_DDR_ZEHNER  	PWM_DDR2
	#endif
	#define ANZ_PORT_EINER		PWM_PORT1
	#define ANZ_DDR_EINER 		PWM_DDR1
#else
	// If PWM is active, these ports are overruled by the PWM ports
	#define ANZ_PORT2_ACTIVE	0
	#if ANZ_PORT2_ACTIVE
		#define ANZ_PORT_ZEHNER 	PORTC
		#define ANZ_DDR_ZEHNER  	DDRC
	#endif
	#define ANZ_PORT_EINER		PORTB
	#define ANZ_DDR_EINER 		DDRB
#endif

#define set_ddr_einer(symbol) ANZ_DDR_EINER = (symbol)
#if PWM_PORT2_ACTIVE
#define set_ddr_zehner(symbol) ANZ_DDR_ZEHNER = (symbol)
#endif

// A little bit of Define Magic for compiling/not compiling PWM
#if PWM_ENABLED
	/**
	 * @see pwm_update_port1
	 */
	#define anzeige_pwm_einer(symbol) pwm_update_port1((symbol))
	#if PWM_PORT2_ACTIVE
		/**
		 * @see pwm_update_port1
		 */
		#define anzeige_pwm_zehner(symbol) pwm_update_port2((symbol))
	#elif ANZ_PORT2_ACTIVE
		#define anzeige_pwm_zehner(symbol) ANZ_PORT_ZEHNER = (symbol)
	#else
		#define anzeige_pwm_zehner(symbol)
	#endif
#else
	#define anzeige_pwm_einer(symbol)  ANZ_PORT_EINER  = (symbol)
	#if ANZ_PORT2_ACTIVE
		#define anzeige_pwm_zehner(symbol) ANZ_PORT_ZEHNER = (symbol)
	#else
		#define anzeige_pwm_zehner(symbol)
	#endif
#endif



#endif /* UC_ANZEIGEBOARD_H_ */
