/*
 * anzeigeboard.h (x86 / PC test build)
 *
 * Selected by anzeigeboard.c when compiled with -DTEST (see tests/Makefile).
 * The PC test suite uses exactly the same pin mapping as the microcontroller;
 * the AVR registers (PORTx/DDRx) are plain variables provided by the mocked
 * <avr/io.h> in tests/mock, so the tests can inspect them.
 *
 * WARNING: This file should be only included in anzeigeboard.c!
 */

#ifndef X86_ANZEIGEBOARD_H_
#define X86_ANZEIGEBOARD_H_

#include "../uc/anzeigeboard.h"

#endif /* X86_ANZEIGEBOARD_H_ */
