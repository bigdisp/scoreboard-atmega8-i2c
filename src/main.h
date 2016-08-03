/*
 * main.h
 *
 *  Created on: 10.10.2013
 *  Author: Martin Beckmann
 */

#ifndef MAIN_H_
#define MAIN_H_

//Falls CPU noch nicht definiert war, hier definieren...
#ifndef F_CPU
#define F_CPU 8000000UL     //8MHz intern
#endif
#ifndef FOSC
#define FOSC 8000000		// Clock Speed
#endif

#include "anzeigeboard.h"
#include "i2c.h"

/**
 * Conversion helper 8 bit <-> 16 bit.
 */
typedef union conv_8_16{
	uint8_t byte[2];
	uint16_t word;
} conv_8bit_16bit;

#endif /* MAIN_H_ */
