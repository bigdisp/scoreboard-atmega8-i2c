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

/**
 * Scoreboard specific IDs.
 *
 * Each value corresponds to a specific seven segment element.
 * This may need change when OUT/STRIKE/BALL are combined into dots, forming one virtual segment.
 * TODO: Addressing the correct chip without specific firmware on each still is an open question.
 */
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

/**
 * Helper for populating sevensegment screen manually.
 *
 * Format is:
 *         AAAAAAA
 *        F       B
 *        F       B
 *        F       B
 *         GGGGGGG
 *        E       C
 *        E       C
 *        E       C
 *         DDDDDDD   PT
 */
typedef union _sevensegment_screen
{
	uint8_t bitmask; ///< Resulting bitmask
	struct
	{
		uint8_t seg_a:1; ///< Top segment
		uint8_t seg_b:1; ///< Top right segment
		uint8_t seg_c:1; ///< Bottom right segment
		uint8_t seg_d:1; ///< Bottom segment
		uint8_t seg_e:1; ///< Bottom left segment
		uint8_t seg_f:1; ///< Top left segment
		uint8_t seg_g:1; ///< Middle segment
		uint8_t seg_pt:1; ///< Additional Point
	};
} u_sevensegment_screen;

//TODO: Anpassen fuer Prototyp
/**
 * Segment definitions for display. These must be adjusted if outputs are connected differently.
 */


/**
 * Write letter or number to seven segment display.
 *
 * Can write two digit numbers to two ports.
 * This function assumes identical pinout on two ports and can only output numbers.
 * @param zahl The number to output
 * @param enable_point True, if the point (LED_PT) should be enabled.
 * @return 1
 */
uint8_t anzeige_write(uint8_t zahl, uint8_t enable_point);


/**
 * Take an ASCII Symbol, convert it to 7 Segment display and display it.
 *
 * Conversions that are unknown are output as dash. A space clears the display.
 * @param symbol The symbol to display on the primary digit (einer)
 * @param upper_symbol The symbol to display on the secondary digit (zehner)
 * @return 1
 */
uint8_t anzeige_write_convert(uint8_t symbol, uint8_t upper_symbol);

/**
 * Take an ASCII Symbol, convert it to 7 Segment display and display it.
 *
 * This function outputs on the lower digit.
 *
 * @see anzeige_write_convert
 */
void anzeige_write_convert_0(uint8_t symbol);

/**
 * Take an ASCII Symbol, convert it to 7 Segment display and display it.
 *
 * This function outputs on the upper digit.
 *
 * @see anzeige_write_convert
 */
void anzeige_write_convert_1(uint8_t symbol);

// anzeige_write_direct will take the binary representation of segments, numbered A-G in LSB -> MSB
/**
 * Write given seven segment binary input and write it to the display.
 *
 * Format is:
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
 * Segment shift of A is assumed 0, segment shift of G is 7, shift of PT is 8.
 * Differing actual shifts due to different wiring are compensated within the function.
 * Therefore, segments can, for example be populated
 *
 * @param segments The segments to enable.
 * @return 1
 */
uint8_t anzeige_write_direct(u_sevensegment_screen segments);

/**
 * Initialize display.
 */
void anzeige_init();

/**
 * Run a sequence of numbers (0-5) across both digits
 */
void anzeige_test();

#endif /* ANZEIGEBOARD_H_ */
