/*
 * anzeigeboard.c
 *
 *  Created on: 01.07.2012
 *      Author: martin
 */

#include "anzeigeboard.h"
#include "pwm.h"

//Segment definititions
//Depend on push-order in schieberegister.c
//If the data is pushed the other way, this needs to be inverted
//#define SEG_A  (1 << 7)
//#define SEG_B  (1 << 6)
//#define SEG_C  (1 << 5)
//#define SEG_D  (1 << 4)
//#define SEG_E  (1 << 3)
//#define SEG_F  (1 << 2)
//#define SEG_G  (1 << 1)
//#define SEG_PT (1 << 0)

//TODO: Anpassen fuer Prototyp
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

#define ANZ_PORT_ZEHNER PORTC
#define ANZ_DDR_ZEHNER  DDRC
#define ANZ_PORT_EINER	PORTB
#define ANZ_DDR_EINER 	DDRB

#define ANZ_DDR 		DDRB

void anzeige_init()
{
	//Everything is an output, except for the nonexisting SEG_PT:
	ANZ_DDR = (0xFF ^ (1 << SEG_PT));
	//TODO: This must be changed for dual display
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
		//*°
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

// A little bit of Define Magic for compiling/not compiling PWM
#if PWM_ENABLED
	#define anzeige_pwm_einer(symbol) pwm_update_port1((symbol))
	#if PWM_PORT2_ACTIVE
		#define anzeige_pwm_zehner(symbol) pwm_update_port2((symbol))
	#else
		#define anzeige_pwm_zehner(symbol) ANZ_PORT_ZEHNER = (symbol)
	#endif
#else
	#define anzeige_pwm_einer(symbol)  ANZ_PORT_EINER  = (symbol)
	#define anzeige_pwm_zehner(symbol) ANZ_PORT_ZEHNER = (symbol)
#endif


uint8_t anzeige_write(uint8_t zahl)
{
	//Split necessary:
	uint8_t zehner, einer;
	zehner = zahl / 10;
	einer  = zahl - (zehner * 10);

	if(zehner > 9)
		zehner = 9;

	//Now simply turn on the GPIO Ports accordingly
	anzeige_pwm_zehner(anzeige_convert(zehner));
	anzeige_pwm_einer(anzeige_convert(einer));
	return 1;
}

uint8_t anzeige_write_convert(uint8_t symbol)
{
	anzeige_pwm_einer(anzeige_convert(symbol));
	return 1;
}

uint8_t anzeige_write_direct(uint8_t segments)
{
	uint8_t tmp = 0;
	// Resort according to segments, assume numbering from A to G LSB - MSB
	// So check for set bit, shift result to zero and shift to correct bit
	tmp |= (segments & (1 << 0)) ? SEG_A : 0;
	tmp |= (segments & (1 << 1)) ? SEG_B : 0;
	tmp |= (segments & (1 << 2)) ? SEG_C : 0;
	tmp |= (segments & (1 << 3)) ? SEG_D : 0;
	tmp |= (segments & (1 << 4)) ? SEG_E : 0;
	tmp |= (segments & (1 << 5)) ? SEG_F : 0;
	tmp |= (segments & (1 << 6)) ? SEG_G : 0;
	tmp |= (segments & (1 << 7)) ? SEG_PT: 0;

	anzeige_pwm_einer(tmp);
	return 1;
}
