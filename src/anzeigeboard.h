/*
 * anzeigeboard.h
 *
 *  Created on: 01.07.2012
 *      Author: martin
 */

#ifndef ANZEIGEBOARD_H_
#define ANZEIGEBOARD_H_

//#include <inttypes.h>
#include <avr/io.h>

typedef enum _anzeigen
{
	E_ANZ_OUT = 6,
	E_ANZ_STRIKE = 7,
	E_ANZ_BALL = 8,

	E_ANZ_RUNS_HEIM = 1,
	E_ANZ_RUNS_GAST = 3,
	E_ANZ_INNING = 5,

	E_ANZ_HITS_HEIM = 10,
	E_ANZ_HITS_GAST = 12,
	E_ANZ_ERRORS_HEIM = 14,
	E_ANZ_ERRORS_GAST = 16,

	//Duplicates for second digit (not necessary for external input)
	E_ANZ_RUNS_HEIM_10 = 0,
	E_ANZ_RUNS_GAST_10 = 2,
	E_ANZ_INNING_10 = 4,

	E_ANZ_HITS_HEIM_10 = 9,
	E_ANZ_HITS_GAST_10 = 11,
	E_ANZ_ERRORS_HEIM_10 = 13,
	E_ANZ_ERRORS_GAST_10 = 15,

} e_anzeigen;

// This function assumes identical pinout on two ports and can only output numbers
uint8_t anzeige_write(uint8_t zahl);

// These functions can output anything.
// anzeige_write_convert will take an ASCII Symbol and convert it to 7 Segment display (if possible)
uint8_t anzeige_write_convert(uint8_t symbol);
// anzeige_write_direct will take the binary representation of segments, numbered A-G in LSB -> MSB
uint8_t anzeige_write_direct(uint8_t segments);
void anzeige_init();

#endif /* ANZEIGEBOARD_H_ */
