/*
 * Host mock of <avr/wdt.h>. Enabling the watchdog is how the firmware resets
 * into the bootloader; the mock records it and leaves the firmware main().
 */
#ifndef MOCK_AVR_WDT_H
#define MOCK_AVR_WDT_H

#define WDTO_15MS 0

void mock_wdt_enable(int timeout);
#define wdt_enable(t)  mock_wdt_enable(t)
#define wdt_disable()  ((void) 0)
#define wdt_reset()    ((void) 0)

#endif
