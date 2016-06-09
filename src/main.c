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
#include "pwm.h"
#include "eeprom.h"

#define I2C_DATA_SIZE 4

int main()
{
	//Anzeige
	anzeige_init();

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
	uint16_t pwm_on = 1000;
	uint16_t pwm_off = 3000;
	pwm_init(pwm_on,pwm_off); //Initialize PWM at 50% and CLK / 2000 Cycles (for 1 MHz -> 500 Hz PWM)
	pwm_enable(1);

	//Second Row of lights enabled:
	uint8_t second_row = 1;
	anzeige_write_convert('-', second_row);

	// Enable Interrupts
	sei();


	//MAIN LOOP
	uint8_t data;
	u_i2c_status i2c_st_reg;
	//uint8_t loop_ctr = 0;

	while(1)
	{
		/*loop_ctr++;
		if(loop_ctr == 0)
		{
			pwm_on = pwm_on + 10;
			pwm_set_timing(pwm_on, pwm_off);
		}*/
		data = i2c_wait_busy();
		i2c_st_reg = i2c_get_status_register();
		if(i2c_st_reg.last_txrx_ok)
		{
			i2c_st_reg = i2c_get_status_register();
			if(i2c_st_reg.rx_data_in_buf)
			{
				i2c_get_data(i2c_msg, I2C_DATA_SIZE);

				// Check for special opcodes to control PWM:
				if(i2c_msg[0] == 0x00 || i2c_msg[0] == 0x01)
				{
					conv_8bit_16bit tmp;
					tmp.byte[0] = i2c_msg[1];
					tmp.byte[1] = i2c_msg[2];
					if(i2c_msg[0] == 0x00)
					{
						pwm_on = tmp.word;
					}
					else
					{
						pwm_off = tmp.word;
					}

					i2c_msg[0] = (pwm_on >> 8) & 0xFF;
					i2c_msg[1] = (pwm_on) & 0xFF;
					i2c_msg[2] = (pwm_off >> 8) & 0xFF;
					i2c_msg[3] = (pwm_off) & 0xFF;
				}
				//Disable PWM?
				else if(i2c_msg[0] == 0x02)
				{
					pwm_enable(0);
				}
				//Reenable PWM?
				else if(i2c_msg[0] == 0x03)
				{
					pwm_enable(1);
					if(i2c_msg[1] == 0x01)
					{
						pwm_on  = 1000;
						pwm_off = 1000;
						i2c_msg[1] = 0x00;
						pwm_set_timing(pwm_on, pwm_off);
					}

				}
				else if(i2c_msg[0] == 0x04)
				{
					if(i2c_msg[1] == 0x01)
					{
						pwm_on  = 1000;
						pwm_off = 1000;
						i2c_msg[1] = 0x00;
					}
					pwm_set_timing(pwm_on, pwm_off);
				}
				else if(i2c_msg[0] == 0x05)
				{
					pwm_on = 1000;
					pwm_off = 1000;
					pwm_set_timing(pwm_on, pwm_off);
				}
				else if(i2c_msg[0] == 0x06)
				{
					pwm_on = 1000;
					pwm_off = 2000;
					pwm_set_timing(pwm_on, pwm_off);
				}
				else if(i2c_msg[0] == 0x07)
				{
					pwm_on = 1000;
					pwm_off = 3000;
					pwm_set_timing(pwm_on, pwm_off);
				}
				else if(i2c_msg[0] == 0x08)
				{
					second_row = i2c_msg[1];
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
