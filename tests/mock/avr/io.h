/*
 * Host mock of <avr/io.h> for the PC test suite.
 * Registers are plain variables (defined in tests/mock/avr_mock.c) so tests
 * can set and inspect them. Only what the firmware uses is provided.
 */
#ifndef MOCK_AVR_IO_H
#define MOCK_AVR_IO_H

#include <stdint.h>

extern volatile uint8_t PORTB, PORTC, PORTD;
extern volatile uint8_t DDRB, DDRC, DDRD;
extern volatile uint8_t TWAR, TWCR;
extern volatile uint8_t mock_MCUSR;
#define MCUSR mock_MCUSR /* macro, like in avr-libc (firmware uses #ifdef MCUSR) */

#define PB0 0
#define PB1 1
#define PB2 2
#define PB3 3
#define PB4 4
#define PB5 5
#define PB6 6
#define PB7 7

#define PC0 0
#define PC1 1
#define PC2 2
#define PC3 3
#define PC4 4
#define PC5 5
#define PC6 6

#define PD0 0
#define PD1 1
#define PD2 2
#define PD3 3
#define PD4 4
#define PD5 5
#define PD6 6
#define PD7 7

#endif
