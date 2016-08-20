/*
 * anzeigeboard.c
 *
 *  Created on: 01.07.2012
 *      Author: martin
 */
#define __IN_ANZEIGEBOARD_C

#include "anzeigeboard.h"
#include "pwm.h"
#include <util/delay.h>

// Test abstraction
#ifdef TEST
	#include "x86/anzeigeboard.h"
#else
	#include "uc/anzeigeboard.h"
#endif


void anzeige_init()
{

	// Set all segments to output:

	// All OFF
	POR_0A &= ~SEG_0A;
	POR_0B &= ~SEG_0B;
	POR_0C &= ~SEG_0C;
	POR_0D &= ~SEG_0D;
	POR_0E &= ~SEG_0E;
	POR_0F &= ~SEG_0F;
	POR_0G &= ~SEG_0G;


	// Digit 1
	DDR_0A |= SEG_0A;
	DDR_0B |= SEG_0B;
	DDR_0C |= SEG_0C;
	DDR_0D |= SEG_0D;
	DDR_0E |= SEG_0E;
	DDR_0F |= SEG_0F;
	DDR_0G |= SEG_0G;

	// Digit 2
	DDR_1A |= SEG_1A;
	DDR_1B |= SEG_1B;
	DDR_1C |= SEG_1C;
	DDR_1D |= SEG_1D;
	DDR_1E |= SEG_1E;
	DDR_1F |= SEG_1F;
	DDR_1G |= SEG_1G;

	// Enable
	// Digit 1
	/*POR_0A |= SEG_0A;
	POR_0B |= SEG_0B;
	POR_0C |= SEG_0C;
	POR_0D |= SEG_0D;
	POR_0E |= SEG_0E;
	POR_0F |= SEG_0F;
	POR_0G |= SEG_0G;

	// Digit 2
	POR_1A |= SEG_1A;
	POR_1B |= SEG_1B;
	POR_1C |= SEG_1C;
	POR_1D |= SEG_1D;
	POR_1E |= SEG_1E;
	POR_1F |= SEG_1F;
	POR_1G |= SEG_1G;//*/
}

/*
 * Converts the given number to a seven segment representation
 * Also interprets many ASCII letters.
 */

uint8_t anzeige_convert(uint8_t ziffer)
{
	uint8_t retval = 0;
	switch(ziffer)
	{
	case 0:
	case 48:
		retval = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
		break;
	case 1:
	case 49:
		retval = SEG_B | SEG_C;
		break;
	case 2:
	case 50:
		retval = SEG_A | SEG_B | SEG_G | SEG_E | SEG_D;
		break;
	case 3:
	case 51:
		retval = SEG_A | SEG_B | SEG_C | SEG_G | SEG_D;
		break;
	case 4:
	case 52:
		retval = SEG_F | SEG_G | SEG_B | SEG_C;
		break;
	case 5:
	case 53:
		retval = SEG_A | SEG_F | SEG_G | SEG_C | SEG_D;
		break;
	case 6:
	case 54:
		retval = SEG_A | SEG_F | SEG_E | SEG_D | SEG_C | SEG_G;
		break;
	case 7:
	case 55:
		retval = SEG_A | SEG_B | SEG_C;
		break;
	case 8:
	case 56:
		retval = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
		break;
	case 9:
	case 57:
		retval = SEG_A | SEG_B | SEG_C | SEG_F | SEG_G | SEG_D;
		break;
	// Buchstaben
	case 65:
	case 97:
		//A
		retval = SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G;
		break;
	case 17:
	case 66:
	case 98:
		//b
		retval = SEG_F | SEG_E | SEG_G | SEG_C | SEG_D;
		break;
	case 18:
	case 67:
	case 99:
		//C
		retval = SEG_A | SEG_F | SEG_E | SEG_D;
		break;
	case 68:
	case 100:
		//d
		retval = SEG_B | SEG_C | SEG_D | SEG_E | SEG_G;
		break;
	case 10:
	case 69:
	case 101:
		//E
		retval = SEG_A | SEG_F | SEG_G | SEG_E | SEG_D;
		break;
	case 70:
	case 102:
		//F
		retval = SEG_A | SEG_E | SEG_F | SEG_G;
		break;
	case 71:
	case 103:
		//G
		retval = SEG_A | SEG_C | SEG_D | SEG_E | SEG_F;
		break;
	case 16:
	case 72:
	case 104:
		//H
		retval = SEG_F | SEG_G | SEG_E | SEG_B | SEG_C;
		break;
	case 13:
	case 73:
	case 105:
		//i
		retval = SEG_E;
		break;
	case 74:
	case 106:
		//J
		retval = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E;
		break;
	case 75:
	case 107:
		//K
		// nicht darstellbar
		retval = SEG_F | SEG_G;
		break;
	case 15:
	case 76:
	case 108:
		//L
		retval = SEG_F | SEG_E | SEG_D;
		break;
	case 77:
	case 109:
		//M
		// nicht darstellbar
		retval = SEG_A | SEG_B | SEG_C | SEG_E | SEG_F;
		break;
	case 78:
	case 110:
		//n
		retval = SEG_C | SEG_E | SEG_G;
		break;
	case 12:
	case 79:
	case 111:
		//o
		retval = SEG_G | SEG_C | SEG_D | SEG_E;
		break;
	case 80:
	case 112:
		//P
		retval = SEG_A | SEG_B | SEG_E | SEG_F | SEG_G;
		break;
	case 81:
	case 113:
		//Q
		retval = SEG_A | SEG_B | SEG_C | SEG_F | SEG_G;
		break;
	case 11:
	case 114:
	case 82:
		//r
		retval = SEG_G | SEG_E;
		break;
	case 115:
	case 83:
		//S
		retval = SEG_A | SEG_C | SEG_D | SEG_F | SEG_G;
		break;
	case 14:
	case 116:
	case 84:
		//t
		retval = SEG_F | SEG_G | SEG_E | SEG_D;
		break;
	case 19:
	case 117:
	case 85:
		//V
		retval = SEG_B | SEG_E | SEG_D | SEG_C | SEG_F;
		break;
	case 118:
	case 86:
		//v
		// nicht von u zu unterscheiden
		retval = SEG_E | SEG_D | SEG_C;
		break;
	case 119:
	case 87:
		//w
		//nicht darstellbar
		retval = SEG_C | SEG_F;
		break;
	case 120:
	case 88:
		//x
		// nicht darstellbar
		retval = SEG_B | SEG_C | SEG_E | SEG_F;
		break;
	case 121:
	case 89:
		//Y
		retval = SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;
		break;
	case 122:
	case 90:
		//Z
		// nicht von 2 zu unterscheiden
		retval = SEG_A | SEG_B | SEG_G | SEG_E | SEG_D;
		break;
	//Symbole
	case 32:
		// Leerzeichen
		retval = 0;
		break;
	case 46:
	case 44:
	case 58:
	case 59:
		//Punktaehnliches:
		retval = SEG_PT;
		break;
	case 42:
		//*�
		retval = SEG_A | SEG_B | SEG_G | SEG_F;
		break;
	case 61:
		//=
		retval = SEG_G | SEG_D;
		break;
	case 95:
		//_
		retval = SEG_D;
		break;
	case 250:
		// 0xFA: Default value, usually u acute
		retval = SEG_C | SEG_D | SEG_E | SEG_A;
		break;
	case 45:
	default:
		//Return a "-"
		retval = SEG_G;
		break;
	}
	return retval;
}

/**
 * Output bitmask given in software bitorder to lower digit.
 * This function converts the bitmask given to the bitmask needed for output.
 */
void anzeige_einer(uint8_t bitmask)
{
	// Resort according to segments, assume numbering from A to G LSB - MSB
	// So check for set bit, shift result to zero and shift to correct bit
	POR_0A = (bitmask & SEG_A) ? (POR_0A | SEG_0A) : (POR_0A & ~SEG_0A);
	POR_0B = (bitmask & SEG_B) ? (POR_0B | SEG_0B) : (POR_0B & ~SEG_0B);
	POR_0C = (bitmask & SEG_C) ? (POR_0C | SEG_0C) : (POR_0C & ~SEG_0C);
	POR_0D = (bitmask & SEG_D) ? (POR_0D | SEG_0D) : (POR_0D & ~SEG_0D);
	POR_0E = (bitmask & SEG_E) ? (POR_0E | SEG_0E) : (POR_0E & ~SEG_0E);
	POR_0F = (bitmask & SEG_F) ? (POR_0F | SEG_0F) : (POR_0F & ~SEG_0F);
	POR_0G = (bitmask & SEG_G) ? (POR_0G | SEG_0G) : (POR_0G & ~SEG_0G);
}

/**
 * Output bitmask given in software bitorder to higher digit.
 * This function converts the bitmask given to the bitmask needed for output.
 */
void anzeige_zehner(uint8_t bitmask)
{
	// Resort according to segments, assume numbering from A to G LSB - MSB
	// So check for set bit, shift result to zero and shift to correct bit
	POR_1A = (bitmask & SEG_A) ? (POR_1A | SEG_1A) : (POR_1A & ~SEG_1A);
	POR_1B = (bitmask & SEG_B) ? (POR_1B | SEG_1B) : (POR_1B & ~SEG_1B);
	POR_1C = (bitmask & SEG_C) ? (POR_1C | SEG_1C) : (POR_1C & ~SEG_1C);
	POR_1D = (bitmask & SEG_D) ? (POR_1D | SEG_1D) : (POR_1D & ~SEG_1D);
	POR_1E = (bitmask & SEG_E) ? (POR_1E | SEG_1E) : (POR_1E & ~SEG_1E);
	POR_1F = (bitmask & SEG_F) ? (POR_1F | SEG_1F) : (POR_1F & ~SEG_1F);
	POR_1G = (bitmask & SEG_G) ? (POR_1G | SEG_1G) : (POR_1G & ~SEG_1G);
}


uint8_t anzeige_write(uint8_t zahl)
{
	//Split necessary:
	uint8_t zehner, einer;
	zehner = zahl / 10;
	einer  = zahl - (zehner * 10);

	if(zehner > 9)
		zehner = 9;

	zehner = anzeige_convert(zehner == 0 ? ' ' : zehner);
	einer  = anzeige_convert(einer);


	//Now simply turn on the GPIO Ports accordingly
	// zehner will be ignored if the second output port is disabled.
	anzeige_zehner(zehner);
	anzeige_einer(einer);
	return 1;
}

void anzeige_write_convert_0(uint8_t symbol)
{
	anzeige_einer(anzeige_convert(symbol));
}
void anzeige_write_convert_1(uint8_t symbol)
{
	anzeige_zehner(anzeige_convert(symbol));
}
uint8_t anzeige_write_convert(uint8_t symbol, uint8_t symbol_upper)
{
	anzeige_write_convert_0(symbol);
	anzeige_write_convert_1(symbol_upper);
	return 1;
}

uint8_t anzeige_write_direct(u_sevensegment_screen segments)
{
	uint8_t tmp = 0;
	// Resort according to segments, assume numbering from A to G LSB - MSB
	// So check for set bit, shift result to zero and shift to correct bit
	tmp |= (segments.bitmask & SEG_A) ? SEG_A : 0;
	tmp |= (segments.bitmask & SEG_B) ? SEG_B : 0;
	tmp |= (segments.bitmask & SEG_C) ? SEG_C : 0;
	tmp |= (segments.bitmask & SEG_D) ? SEG_D : 0;
	tmp |= (segments.bitmask & SEG_E) ? SEG_E : 0;
	tmp |= (segments.bitmask & SEG_F) ? SEG_F : 0;
	tmp |= (segments.bitmask & SEG_G) ? SEG_G : 0;
	tmp |= (segments.bitmask & SEG_PT) ? SEG_PT: 0;

	anzeige_einer(tmp);
	return 1;
}

void anzeige_set_bits_0(uint8_t bit)
{
	if (bit & SEG_A) POR_0A |= SEG_0A;
	if (bit & SEG_B) POR_0B |= SEG_0B;
	if (bit & SEG_C) POR_0C |= SEG_0C;
	if (bit & SEG_D) POR_0D |= SEG_0D;
	if (bit & SEG_E) POR_0E |= SEG_0E;
	if (bit & SEG_F) POR_0F |= SEG_0F;
	if (bit & SEG_G) POR_0G |= SEG_0G;
}

void anzeige_clear_bits_0(uint8_t bit)
{
	if (bit & SEG_A) POR_0A &= ~(SEG_0A);
	if (bit & SEG_B) POR_0B &= ~(SEG_0B);
	if (bit & SEG_C) POR_0C &= ~(SEG_0C);
	if (bit & SEG_D) POR_0D &= ~(SEG_0D);
	if (bit & SEG_E) POR_0E &= ~(SEG_0E);
	if (bit & SEG_F) POR_0F &= ~(SEG_0F);
	if (bit & SEG_G) POR_0G &= ~(SEG_0G);
}
void anzeige_set_bits_1(uint8_t bit)
{
	if (bit & SEG_A) POR_1A |= SEG_1A;
	if (bit & SEG_B) POR_1B |= SEG_1B;
	if (bit & SEG_C) POR_1C |= SEG_1C;
	if (bit & SEG_D) POR_1D |= SEG_1D;
	if (bit & SEG_E) POR_1E |= SEG_1E;
	if (bit & SEG_F) POR_1F |= SEG_1F;
	if (bit & SEG_G) POR_1G |= SEG_1G;
}

void anzeige_clear_bits_1(uint8_t bit)
{
	if (bit & SEG_A) POR_1A &= ~(SEG_1A);
	if (bit & SEG_B) POR_1B &= ~(SEG_1B);
	if (bit & SEG_C) POR_1C &= ~(SEG_1C);
	if (bit & SEG_D) POR_1D &= ~(SEG_1D);
	if (bit & SEG_E) POR_1E &= ~(SEG_1E);
	if (bit & SEG_F) POR_1F &= ~(SEG_1F);
	if (bit & SEG_G) POR_1G &= ~(SEG_1G);
}



/**
 * Test output by running a sequence of 0-5 across the two digits.
 */
void anzeige_test()
{
	// Off:
	anzeige_einer(0);
	anzeige_zehner(0);
	_delay_ms(1000);
	anzeige_einer(anzeige_convert(0x30));
	anzeige_zehner(anzeige_convert(0x30));
	_delay_ms(800);
	anzeige_einer(anzeige_convert(0x31));
	anzeige_zehner(anzeige_convert(0x31));
	_delay_ms(800);
	anzeige_einer(anzeige_convert(0x32));
	anzeige_zehner(anzeige_convert(0x32));
	_delay_ms(800);
	anzeige_einer(anzeige_convert(0x33));
	anzeige_zehner(anzeige_convert(0x33));
	_delay_ms(800);
	anzeige_einer(anzeige_convert(0x34));
	anzeige_zehner(anzeige_convert(0x34));
	_delay_ms(800);
	anzeige_einer(anzeige_convert(0x35));
	anzeige_zehner(anzeige_convert(0x35));
	_delay_ms(800);
	anzeige_einer(anzeige_convert(0x38));
	anzeige_zehner(anzeige_convert(0x38));
	_delay_ms(1000);
	anzeige_einer(0);
	anzeige_zehner(0);
}
