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
#include <util/delay.h>
#include "pwm.h"
#include "eeprom.h"

#define I2C_DATA_SIZE 4




int main()
{
	//###############################################
	//Konfigurationsregister etc. einstellen

	//initConfig();
	anzeige_init();

	// PWM
	pwm_init();
	pwm_enable(1);
	pwm_set_timing_b(200, 0);
	pwm_set_timing_r(200, 0);
	pwm_set_timing_g(511, 0);
	pwm_set_timing_b(200, 1);
	pwm_set_timing_r(200, 1);
	pwm_set_timing_g(511, 1);

	//i2c / TWI
	uint8_t slave_addr;
	slave_addr = eeprom_read_byte(0);

	// Check for illegal addresses:
	if (slave_addr >= 0xF0 || slave_addr <= 0x0F)
	{
		// Set the default address in this case:
		slave_addr = 0x10;
	}

	i2c_init(slave_addr);
	uint8_t i2c_msg[I2C_DATA_SIZE] = {0xFA, 0xFA, 0xFA, 0xFA};
	i2c_slave_start_tx_data(i2c_msg,3);

	// PWM
	uint8_t  pwm_r[2] = {100, 100};
	uint16_t pwm_g[2] = {250, 250};
	uint8_t  pwm_b[2] = {100, 100};
	//pwm_init();
	//pwm_enable(1);


	//Second Row of lights enabled:
	uint8_t second_row = 1;
//	anzeige_write_convert('-', second_row);

//	anzeige_write(88, second_row);

	// Test output
	anzeige_test();

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

				// Check for special opcodes to control PWM:
				// Segment 0
				if(i2c_msg[0] == 0x00 || i2c_msg[0] == 0x01)
				{
					if(i2c_msg[0] == 0x00)
					{
						pwm_r[0] = i2c_msg[1];
						pwm_b[0] = i2c_msg[2];
						pwm_set_timing_r(pwm_r[0], 0);
						pwm_set_timing_b(pwm_b[0], 0);
					}
					else
					{
						conv_8bit_16bit tmp;
						tmp.byte[0] = i2c_msg[1];
						tmp.byte[1] = i2c_msg[2];
						pwm_g[0] = tmp.word;
						pwm_set_timing_g(pwm_g[0], 0);
					}

					i2c_msg[0] = (pwm_r[0]);
					i2c_msg[1] = (pwm_b[0]);
					i2c_msg[2] = (pwm_g[0] >> 8) & 0xFF;
					i2c_msg[3] = (pwm_g[0]) & 0xFF;
				}
				// Segment 1
				else if(i2c_msg[0] == 0x02 || i2c_msg[0] == 0x03)
				{
					if(i2c_msg[0] == 0x02)
					{
						pwm_r[1] = i2c_msg[1];
						pwm_b[1] = i2c_msg[2];
						pwm_set_timing_r(pwm_r[1], 1);
						pwm_set_timing_b(pwm_b[1], 1);
					}
					else
					{
						conv_8bit_16bit tmp;
						tmp.byte[0] = i2c_msg[1];
						tmp.byte[1] = i2c_msg[2];
						pwm_g[1] = tmp.word;
						pwm_set_timing_g(pwm_g[1], 1);
					}

					i2c_msg[0] = (pwm_r[1]);
					i2c_msg[1] = (pwm_b[1]);
					i2c_msg[2] = (pwm_g[1] >> 8) & 0xFF;
					i2c_msg[3] = (pwm_g[1]) & 0xFF;
				}
				//Disable PWM?
				else if(i2c_msg[0] == 0x04)
				{
					pwm_enable(0);
				}
				//Reenable PWM?
				else if(i2c_msg[0] == 0x05)
				{
					pwm_enable(1);
				}
				//Output number on both digits
				else if(i2c_msg[0] == 0x06)
				{
					anzeige_write(i2c_msg[1], second_row);
				}
				else if(i2c_msg[0] == 0x07)
				{
					anzeige_write_convert_0(i2c_msg[1]);
				}
				else if(i2c_msg[0] == 0x08)
				{
					second_row = i2c_msg[1];
					anzeige_write_convert_1(i2c_msg[1]);
				}
				// Set individual bits (for balls/strikes/outs)
				else if(i2c_msg[0] == 0x09)
				{
					anzeige_set_bits_0(i2c_msg[1]);
				}
				else if(i2c_msg[0] == 0x0A)
				{
					anzeige_clear_bits_0(i2c_msg[1]);
				}
				else if(i2c_msg[0] == 0x0B)
				{
					anzeige_set_bits_1(i2c_msg[1]);
				}
				else if(i2c_msg[0] == 0x0C)
				{
					anzeige_clear_bits_1(i2c_msg[1]);
				}
				else
				{
					//Whatever is in the first slot, use as output:
					anzeige_write_convert(i2c_msg[0], second_row);

				}
			}

		}


		//Always set in rxtx mode again afterwards:
		i2c_slave_start_tx_data(i2c_msg, I2C_DATA_SIZE);


	}

	//Wird nicht erreicht
	return 0;
}
