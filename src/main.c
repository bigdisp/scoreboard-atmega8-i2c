/*
 * main.c
 *
 *  Created on: 10.10.2013
 *      Author: martin
 */

#include "main.h"
#include "anzeigeboard.h"
#include "i2c.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define I2C_DATA_SIZE 4

int main()
{
	//INIT
	anzeige_init();
	anzeige_write_convert('-');

	//i2c / TWI
	i2c_init(0x10);
	uint8_t i2c_msg[I2C_DATA_SIZE] = {0xFA, 0xFA, 0xFA, 0xFA};
	i2c_slave_start_tx_data(i2c_msg,3);

	// Enable Interrupts
	sei();


	//MAIN LOOP
	uint8_t data;
	u_i2c_status i2c_st_reg;
	while(1)
	{
		data = i2c_wait_busy();
		i2c_st_reg = i2c_get_status_register();
		if(i2c_st_reg.last_txrx_ok)
		{
			i2c_st_reg = i2c_get_status_register();
			if(i2c_st_reg.rx_data_in_buf)
			{
				i2c_get_data(i2c_msg, I2C_DATA_SIZE);
			}

		}
		//Whatever is in the first slot, use as output:
		anzeige_write_convert(i2c_msg[0]);

		//Always set in rxtx mode again afterwards:
		i2c_slave_start_tx_data(i2c_msg, I2C_DATA_SIZE);


	}

	//Wird nicht erreicht
	return 0;
}
