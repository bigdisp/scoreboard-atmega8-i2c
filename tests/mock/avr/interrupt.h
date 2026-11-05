/* Host mock of <avr/interrupt.h> for the PC test suite. */
#ifndef MOCK_AVR_INTERRUPT_H
#define MOCK_AVR_INTERRUPT_H

extern int mock_irq_enabled;
#define sei() (mock_irq_enabled = 1)
#define cli() (mock_irq_enabled = 0)
#define ISR(vector) void vector(void)

#endif
