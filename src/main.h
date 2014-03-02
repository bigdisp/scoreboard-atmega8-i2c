/*
 * main.h
 *
 *  Created on: 10.10.2013
 *      Author: martin
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "anzeigeboard.h"
#include "i2c.h"

typedef union conv_8_16{
	uint8_t byte[2];
	uint16_t word;
} conv_8bit_16bit;

#endif /* MAIN_H_ */
