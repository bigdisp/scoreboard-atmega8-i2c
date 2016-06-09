/*
 * eeprom.c
 *
 *  Created on: 11.05.2016
 *  Author: Martin Beckmann
 */

#include "eeprom.h"



void irq_ctrl(uint8_t enable)
{
	static uint8_t tmp_sreg = 0;
	if (enable)
	{
		SREG = tmp_sreg;
	}
	else
	{
		tmp_sreg = SREG;
		cli();
	}
}

// Interrupt disabling implementations:

/**
 * Read a single byte from the given address in eeprom.
 */
uint8_t irqd_eeprom_read_byte(const uint8_t * ptr)
{
	uint8_t retval;
	irq_ctrl(0);
	retval = eeprom_read_byte(ptr);
	irq_ctrl(1);
	return retval;
}

/**
 * Read one 16 bit word from given address in eeprom.
 */
uint16_t irqd_eeprom_read_word(const uint16_t * ptr)
{
	uint16_t retval;
	irq_ctrl(0);
	retval = eeprom_read_word(ptr);
	irq_ctrl(1);
	return retval;
}

/**
 * Read one 32 bit dword from given address in eeprom.
 */
uint32_t irqd_eeprom_read_dword(const uint32_t * ptr)
{
	uint32_t retval;
	irq_ctrl(0);
	retval = eeprom_read_dword(ptr);
	irq_ctrl(1);
	return retval;
}

/**
 * Read 32 bit float from eeprom.
 */
float irqd_eeprom_read_float(const float * ptr)
{
	float retval;
	irq_ctrl(0);
	retval = eeprom_read_float(ptr);
	irq_ctrl(1);
	return retval;
}

/**
 * Read an entire block of size sz from src in eeprom to dst in sram.
 */
void irqd_eeprom_read_block(void * dst, void * src, uint8_t sz)
{
	irq_ctrl(0);
	eeprom_read_block(dst, src, sz);
	irq_ctrl(1);
}

/**
 * Write byte value to address ptr in eeprom.
 *
 * To avoid unneccessary writes, use update instead!
 */
void irqd_eeprom_write_byte(uint8_t * ptr, uint8_t value)
{
	irq_ctrl(0);
	eeprom_write_byte(ptr, value);
	irq_ctrl(1);
}

/**
 * Write 16 bit word to address ptr in eeprom.
 *
 * To avoid unneccessary writes, use update instead!
 */
void irqd_eeprom_write_word(uint16_t * ptr, uint16_t value)
{
	irq_ctrl(0);
	eeprom_write_word(ptr, value);
	irq_ctrl(1);
}

/**
 * Write 32 bit dword to address ptr in eeprom.
 *
 * To avoid unneccessary writes, use update instead!
 */
void irqd_eeprom_write_dword(uint32_t * ptr, uint32_t value)
{
	irq_ctrl(0);
	eeprom_write_dword(ptr, value);
	irq_ctrl(1);
}

/**
 * Write 32 bit float to address ptr in eeprom.
 *
 * To avoid unneccessary writes, use update instead!
 */
void irqd_eeprom_write_float(float * ptr, float value)
{
	irq_ctrl(0);
	eeprom_write_float(ptr, value);
	irq_ctrl(1);
}

/**
 * Write data block of size sz from src in sram to dst in eeprom.
 *
 * To avoid unneccessary writes, use update instead!
 */
void irqd_eeprom_write_block(const void * src, void * dst, uint8_t sz)
{
	irq_ctrl(0);
	eeprom_write_block(src, dst, sz);
	irq_ctrl(1);
}

/**
 * Update one byte in eeprom.
 */
void irqd_eeprom_update_byte(uint8_t * ptr, uint8_t value)
{
	irq_ctrl(0);
	eeprom_update_byte(ptr, value);
	irq_ctrl(1);
}

/**
 * Update one 16 bit word in eeprom.
 */
void irqd_eeprom_update_word(uint16_t * ptr, uint16_t value)
{
	irq_ctrl(0);
	eeprom_update_word(ptr, value);
	irq_ctrl(1);
}

/**
 * Update one 32 bit word in eeprom.
 */
void irqd_eeprom_update_dword(uint32_t * ptr, uint32_t value)
{
	irq_ctrl(0);
	eeprom_update_dword(ptr, value);
	irq_ctrl(1);
}
/**
 * Update one 32 bit float in eeprom.
 */
void irqd_eeprom_update_float(float * ptr, float value)
{
	irq_ctrl(0);
	eeprom_update_float(ptr, value);
	irq_ctrl(1);
}

/**
 * Update a block of size sz from src in sram to dst in eeprom.
 */
void irqd_eeprom_update_block(const void * src, void * dst, uint8_t sz)
{
	irq_ctrl(0);
	eeprom_update_block(src, dst, sz);
	irq_ctrl(1);
}
