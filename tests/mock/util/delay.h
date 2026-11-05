/* Host mock of <util/delay.h>: delays are no-ops in the PC test suite. */
#ifndef MOCK_UTIL_DELAY_H
#define MOCK_UTIL_DELAY_H
#define _delay_ms(ms) ((void) (ms))
#define _delay_us(us) ((void) (us))
#endif
