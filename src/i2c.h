/*
 * i2c.h
 *
 *  Created on: 10.10.2013
 *      Author: martin
 */

#ifndef I2C_H_
#define I2C_H_

/// Largest permitted I2C message size in both directions
#define I2C_BUFFER_SIZE 4

/**
 * I2C status structure
 */
typedef union i2c_status_u
{
    unsigned char all;
    struct
    {
        unsigned char last_txrx_ok:1;	///< 1: last tx/rx was successful
        unsigned char rx_data_in_buf:1;	///< 1: Received data is ready for reading
        unsigned char gen_call_rx:1;	///< 1: Data is from general call 0: individual address
        unsigned char unused_bits:5;
    };
} u_i2c_status;

// TODO: Is this really needed? These are equivalent to the basic status codes
/**
 * I2C Status Codes
 */
typedef enum
{
	I2C_ADRESS_RECV 	= 0x60,
	I2C_GCALL_RECV 		= 0x70,
	I2C_DATA_ACK_RECV 	= 0x80,
	I2C_DATA_NACK_RECV	= 0x88,
	I2C_STOP_START_RECV	= 0xA0,

	I2C_NO_STATE 		= 0xF8,
	I2C_BUS_ERROR 		= 0x00,

} e_i2c_status;

/**
 * Initialize i2c interface.
 * @param address The slave address of the i2c interface.
 */
void i2c_init(uint8_t address);

/**
 * Get current status of the i2c interface.
 * @return status code.
 */
uint8_t i2c_get_status();

/**
 * Check whether i2c interface is currently busy (non-blocking).
 */
uint8_t i2c_busy();

/**
 * Wait until i2c interface is no longer busy (blocking).
 * @return status code of i2c interface after waiting.
 */
uint8_t i2c_wait_busy();

/**
 * Get the current status register.
 * @return current i2c status.
 */
u_i2c_status i2c_get_status_register();

/**
 * Set the data to be transmitted via i2c.
 *
 * This also automatically enables receive mode on i2c interface.
 * @param [in] msg Data to transmit.
 * @param size Size of data to transmit.
 */
void i2c_slave_start_tx_data(uint8_t * msg, uint8_t size);

/**
 * Enable i2c receive.
 */
void i2c_slave_start_rx();

/**
 * Return received data.
 *
 * Returned data will be filled with zeros if more data is requested than was received.
 * @param [out] msg Data received.
 * @param [in] size Max size of data returned.
 */
uint8_t i2c_get_data(uint8_t * msg, uint8_t size);

#endif /* I2C_H_ */
