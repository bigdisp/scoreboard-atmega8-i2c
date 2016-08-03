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
// For Software Use only:
#define SEG_A  (1 << 0)
#define SEG_B  (1 << 1)
#define SEG_C  (1 << 2)
#define SEG_D  (1 << 3)
#define SEG_E  (1 << 4)
#define SEG_F  (1 << 5)
#define SEG_G  (1 << 6)
#define SEG_PT (1 << 7)

// Pins & Ports Segment 0
#define P_0A PB0
#define P_0B PB4
#define P_0C PB5
#define P_0D PB6
#define P_0E PB7
#define P_0F PC0
#define P_0G PC1

#define SEG_0A  (1 << PB0)
#define SEG_0B  (1 << PB4)
#define SEG_0C  (1 << PB5)
#define SEG_0D  (1 << PB6)
#define SEG_0E  (1 << PB7)
#define SEG_0F  (1 << PC0)
#define SEG_0G  (1 << PC1)

#define POR_0A PORTB
#define POR_0B PORTB
#define POR_0C PORTB
#define POR_0D PORTB
#define POR_0E PORTB
#define POR_0F PORTC
#define POR_0G PORTC

#define DDR_0A DDRB
#define DDR_0B DDRB
#define DDR_0C DDRB
#define DDR_0D DDRB
#define DDR_0E DDRB
#define DDR_0F DDRC
#define DDR_0G DDRC

// Pins & Ports Segment 1
// In the layout, a few ports are swapped in order
// So we swap them in the software as well to correct
// Order of lines is according to the layout,
// Segments are named according to correction
#define SEG_1C  (1 << PC2)
#define SEG_1B  (1 << PC3)
#define SEG_1A  (1 << PD0)
#define SEG_1E  (1 << PD1)
#define SEG_1D  (1 << PD2)
#define SEG_1G  (1 << PD4)
#define SEG_1F  (1 << PD7)

#define POR_1C PORTC
#define POR_1B PORTC
#define POR_1A PORTD
#define POR_1E PORTD
#define POR_1D PORTD
#define POR_1G PORTD
#define POR_1F PORTD

#define DDR_1C DDRC
#define DDR_1B DDRC
#define DDR_1A DDRD
#define DDR_1E DDRD
#define DDR_1D DDRD
#define DDR_1G DDRD
#define DDR_1F DDRD

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


#endif /* UC_ANZEIGEBOARD_H_ */
