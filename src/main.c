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
#include <avr/wdt.h>
#include <util/delay.h>
#include "pwm.h"
#include "eeprom.h"

#define I2C_DATA_SIZE 4

/// Address used if the eeprom does not contain a valid one
#define I2C_ADDR_DEFAULT 0x10

/// Command sequence that resets the controller into the I2C flash bootloader
#define CMD_ENTER_FBL_0 0x10
#define CMD_ENTER_FBL_1 0x4C
#define CMD_ENTER_FBL_2 0xFB

/// Time the main loop waits when there is nothing to do
#define MAIN_LOOP_IDLE_US 20

/**
 * Make sure the watchdog is off right after reset.
 *
 * The application does not use the watchdog. If it was enabled before
 * (e.g. by a bootloader or after a watchdog reset, where it stays enabled
 * with the shortest timeout), the controller would otherwise reset
 * periodically. This runs before .data/.bss initialization.
 */
void wdt_off(void) __attribute__((naked, used, section(".init3")));
void wdt_off(void)
{
#ifdef MCUSR
	MCUSR = 0;
#else
	MCUCSR = 0;
#endif
	wdt_disable();
}

int main()
{
	// Init Display
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
	if (!I2C_ADDR_VALID(slave_addr))
	{
		// Set the default address in this case:
		slave_addr = I2C_ADDR_DEFAULT;
	}

	// Configure the TWI, but do not answer yet: while interrupts are
	// disabled an addressed TWI would hold SCL low and block the whole bus.
	i2c_init(slave_addr);
	uint8_t i2c_msg[I2C_DATA_SIZE] = {0xFA, 0xFA, 0xFA, 0xFA};
	i2c_slave_set_tx_data(i2c_msg, 3);

	// PWM
	uint8_t  pwm_r[2] = {100, 100};
	uint16_t pwm_g[2] = {250, 250};
	uint8_t  pwm_b[2] = {100, 100};
	//pwm_init();
	//pwm_enable(1);


	//Second Row of lights enabled:
	uint8_t second_row = 1;
//	anzeige_write_convert('-', second_row);

	// Enable Interrupts and start answering on the bus.
	// Commands received during the display test are queued and executed afterwards.
	sei();
	i2c_slave_enable();

	// Test output
	anzeige_test();


	//MAIN LOOP
	while(1)
	{
		uint8_t rx_len = i2c_get_message(i2c_msg, I2C_DATA_SIZE);
		if (0 == rx_len)
		{
			// Nothing received. Watch the bus for stalled transfers.
			_delay_us(MAIN_LOOP_IDLE_US);
			i2c_supervise(MAIN_LOOP_IDLE_US);
			continue;
		}


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
			if(i2c_msg[1])
			{
				pwm_set_output_disabled(i2c_msg[1]);
			}
		}
		//Reenable PWM?
		else if(i2c_msg[0] == 0x05)
		{
			pwm_enable(1);
		}
		//Output number on both digits
		else if(i2c_msg[0] == 0x06)
		{
			anzeige_write(i2c_msg[1]);
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
		// I2C Address change Part 1 (set Address)
		else if(i2c_msg[0] == 0x0D)
		{
			// To make sure we don't change to a false address,
			// Adress must be given twice
			if(i2c_msg[1] == i2c_msg[2])
			{
				// Check for validity
				if(I2C_ADDR_VALID(i2c_msg[1]))
				{
					slave_addr = i2c_msg[1];
				}
			}
		}
		// I2C Address change Part 2 (Temporary change)
		else if(i2c_msg[0] == 0x0E)
		{
			// Require command to contain current slave address
			if(i2c_msg[1] == slave_addr && i2c_msg[2] == slave_addr)
			{
				i2c_set_address(slave_addr);
			}
		}
		// I2C Address change Part 3 (Permanent change)
		else if(i2c_msg[0] == 0x0F)
		{
			uint8_t eepr_data = irqd_eeprom_read_byte(0);
			if (!I2C_ADDR_VALID(eepr_data))
			{
				eepr_data = I2C_ADDR_DEFAULT;
			}
			//anzeige_write_convert(slave_addr, eepr_data);
			anzeige_write_convert(i2c_msg[1], i2c_msg[2]); // bH -> b in 2, h in 1
			if(i2c_msg[1] == slave_addr && (i2c_msg[2] == eepr_data))
			{
				anzeige_write_convert('C', eepr_data);
				// Check if Part 2 has been performed and we are actually using the new address
				if (i2c_get_address() == slave_addr)
				{
					irqd_eeprom_update_byte(0, slave_addr);
					anzeige_write_convert('A', eepr_data);
				}
			}
		}
		// Enter flash bootloader: exactly 0x10 0x4C 0xFB.
		// A plain 0x10 (or any other length) is still displayed as a symbol.
		else if(rx_len == 3 && i2c_msg[0] == CMD_ENTER_FBL_0
				&& i2c_msg[1] == CMD_ENTER_FBL_1 && i2c_msg[2] == CMD_ENTER_FBL_2)
		{
			// Let the watchdog reset the controller. With the BOOTRST fuse
			// programmed the reset starts the bootloader (atmega_i2c_fbl),
			// which disables the watchdog and waits for the flasher.
			// Without a bootloader the application simply restarts
			// (the watchdog is disabled again in wdt_off()).
			cli();
			wdt_enable(WDTO_15MS);
			for(;;)
			{
			}
		}
		else
		{
			//Whatever is in the first slot, use as output:
			anzeige_write_convert(i2c_msg[0], second_row);

		}

		// Data returned on the next read
		i2c_slave_set_tx_data(i2c_msg, I2C_DATA_SIZE);
	}

	//Wird nicht erreicht
	return 0;
}
