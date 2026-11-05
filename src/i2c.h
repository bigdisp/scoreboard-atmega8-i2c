/*
 * i2c.h
 *
 *  Created on: 10.10.2013
 *      Author: martin
 *
 * Interrupt driven I2C (TWI) slave.
 *
 * Received write transactions are put into a small queue by the ISR, so the
 * slave keeps acknowledging its address while the main loop is still busy
 * executing a previous command. Data for read transactions is latched from a
 * transmit buffer that the main loop can update at any time.
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

/// Largest permitted I2C message size in both directions
#define I2C_BUFFER_SIZE 4

/// Number of received messages that can be queued before the slave NACKs
#define I2C_RX_QUEUE_SIZE 4

/// Lowest slave address accepted by the firmware
#define I2C_ADDR_MIN 0x10
/// Highest slave address accepted by the firmware (0x78-0x7F are reserved, TWAR only holds 7 bits)
#define I2C_ADDR_MAX 0x77

/**
 * Check whether the given 7 bit address can be used as slave address.
 */
#define I2C_ADDR_VALID(addr) ((addr) >= I2C_ADDR_MIN && (addr) <= I2C_ADDR_MAX)

/**
 * Initialize i2c interface.
 *
 * Resets the TWI hardware and sets the slave address. The slave does not
 * acknowledge its address until i2c_slave_enable() is called.
 * @param address The 7 bit slave address of the i2c interface.
 */
void i2c_init(uint8_t address);

/**
 * Start answering to the own slave address.
 *
 * Global interrupts should be enabled before (or immediately after) calling this,
 * otherwise the TWI hardware will stretch the clock and block the bus.
 */
void i2c_slave_enable(void);

/**
 * Change the slave address while running.
 *
 * Any transaction in progress is aborted, queued messages are kept.
 * @param address The new 7 bit slave address.
 */
void i2c_set_address(uint8_t address);

/**
 * @return the 7 bit slave address currently used by the hardware.
 */
uint8_t i2c_get_address(void);

/**
 * Set the data returned on the next read transactions.
 *
 * @param [in] msg Data to transmit.
 * @param size Size of data to transmit (truncated to I2C_BUFFER_SIZE).
 */
void i2c_slave_set_tx_data(const uint8_t * msg, uint8_t size);

/**
 * Fetch the oldest received message.
 *
 * Bytes that were not received are set to zero.
 * @param [out] msg Data received.
 * @param [in] size Size of msg.
 * @return number of bytes received in this message, 0 if no message is pending.
 */
uint8_t i2c_get_message(uint8_t * msg, uint8_t size);

/**
 * Supervise the bus. Must be called periodically from the main loop.
 *
 * If a transaction addressed to this slave stalls (e.g. the master was
 * interrupted in the middle of a transfer), the TWI hardware is reset so that
 * SDA/SCL are released again and the bus does not stay blocked.
 * @param elapsed_us Approximate time since the last call in microseconds.
 */
void i2c_supervise(uint16_t elapsed_us);

#endif /* I2C_H_ */
