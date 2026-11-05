/*
 * i2c_private.h
 *
 * State shared between the i2c API (i2c.c) and the TWI interrupt service
 * routine (interrupt.c). Not to be used by anything else.
 */

#ifndef I2C_PRIVATE_H_
#define I2C_PRIVATE_H_

#include <stdint.h>
#include "i2c.h"

#if (I2C_RX_QUEUE_SIZE & (I2C_RX_QUEUE_SIZE - 1)) != 0
	#error I2C_RX_QUEUE_SIZE must be a power of two
#endif

/// One received write transaction
typedef struct
{
	uint8_t len;                      ///< Number of valid bytes in data
	uint8_t data[I2C_BUFFER_SIZE];    ///< Received bytes
} i2c_message_t;

/// TWCR value: TWI enabled, interrupt enabled, clear TWINT, acknowledge
#define I2C_TWCR_ACK  ((1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA))
/// TWCR value: TWI enabled, interrupt enabled, clear TWINT, do not acknowledge
#define I2C_TWCR_NACK ((1 << TWEN) | (1 << TWIE) | (1 << TWINT))

// Receive queue. Written by the ISR at head, read by the main loop at tail.
// head and tail are free running counters, the slot index is counter % size.
extern volatile i2c_message_t i2c_rx_queue[I2C_RX_QUEUE_SIZE];
extern volatile uint8_t i2c_rx_head;
extern volatile uint8_t i2c_rx_tail;

// Transmit data prepared by the main loop, latched by the ISR on SLA+R.
extern volatile uint8_t i2c_tx_next[I2C_BUFFER_SIZE];
extern volatile uint8_t i2c_tx_next_len;

/// 1 while this slave is addressed (between SLA+R/W and STOP/NACK)
extern volatile uint8_t i2c_transfer_active;
/// Incremented on every TWI interrupt, used to detect stalled transfers
extern volatile uint8_t i2c_activity;

#endif /* I2C_PRIVATE_H_ */
