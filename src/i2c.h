/*
 * i2c.h
 *
 *  Created on: 10.10.2013
 *      Author: martin
 */

#ifndef I2C_H_
#define I2C_H_

/**
 * I2C RGN Protokoll
 *
 * 1. Datenbyte: Anzuzeigende Zahl
 * Mehr nicht vorerst
 */

#define I2C_BUFFER_SIZE 4 //Largest Message size including address, so should be at least 2

typedef union i2c_status_u                       // Status byte holding flags.
{
    unsigned char all;
    struct
    {
        unsigned char last_txrx_ok:1;
        unsigned char rx_data_in_buf:1;
        unsigned char gen_call_rx:1;                        // TRUE = General call, FALSE = TWI Address;
        unsigned char unused_bits:5;
    };
} u_i2c_status;

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


void i2c_init(uint8_t address);
uint8_t i2c_get_status();
uint8_t i2c_busy();
uint8_t i2c_wait_busy();
u_i2c_status i2c_get_status_register();

void i2c_slave_start_tx_data(uint8_t * msg, uint8_t size);
void i2c_slave_start_rx();
uint8_t i2c_get_data(uint8_t * msg, uint8_t size);

#endif /* I2C_H_ */
