/* Shared declarations of the host mocks, see avr_mock.c. */
#ifndef AVR_MOCK_H
#define AVR_MOCK_H

#include <stdint.h>
#include <setjmp.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include "i2c.h"
#include "pwm.h"
#include "eeprom.h"

#define MOCK_I2C_MAX_MSGS 64

struct mock_i2c
{
	uint8_t rx_queue[MOCK_I2C_MAX_MSGS][I2C_BUFFER_SIZE]; ///< Messages "sent by the master"
	uint8_t rx_len[MOCK_I2C_MAX_MSGS];
	int rx_count;                           ///< Number of queued messages
	int rx_pos;                             ///< Next message to deliver
	uint8_t tx[I2C_BUFFER_SIZE];            ///< Last data prepared for master read
	uint8_t tx_size;
	uint8_t address;                        ///< Address the TWI currently answers to
	int init_count;
	int set_address_count;
	int enabled;                            ///< i2c_slave_enable() called
	int irq_enabled_at_enable;              ///< interrupts were on when enabled
	int eeprom_writes;
	int wdt_reset;                          ///< firmware requested a watchdog reset
	uint8_t twcr_at_wdt_reset;              ///< TWI control register at that moment
	int irq_enabled_at_wdt_reset;
};

struct mock_pwm
{
	int init_count;
	int enable_calls;
	uint8_t enabled;
	uint8_t output_disabled;
	int output_disabled_calls;
	uint8_t r[2], b[2];
	uint16_t g[2];
};

extern struct mock_i2c mock_i2c;
extern struct mock_pwm mock_pwm;
extern jmp_buf mock_main_exit;

void mock_reset(void);
void mock_i2c_send(const uint8_t *msg, int len);

/// Queue one I2C write transaction of the master, e.g. I2C_SEND(0x06, 42)
#define I2C_SEND(...) mock_i2c_send((const uint8_t[]){ __VA_ARGS__ }, \
	(int) sizeof((const uint8_t[]){ __VA_ARGS__ }))

#endif
