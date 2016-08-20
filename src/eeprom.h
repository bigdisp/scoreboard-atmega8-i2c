/*
 * eeprom.h
 *
 *  Created on: 11.05.2016
 *  Author: Martin Beckmann
 */

#ifndef EEPROM_H_
#define EEPROM_H_
#include <avr/eeprom.h>
#include <avr/interrupt.h>

// Alternatively, use EEMEM attribute?

/**
 * Disable or enable currently enabled/disabled interrupts
 */
void irq_ctrl(uint8_t enable);

/**
 * Read a single byte from the given address in eeprom.
 */
uint8_t irqd_eeprom_read_byte(const uint8_t * ptr);
/**
 * Read one 16 bit word from given address in eeprom.
 */
uint16_t irqd_eeprom_read_word(const uint16_t * ptr);

/**
 * Read one 32 bit dword from given address in eeprom.
 */
uint32_t irqd_eeprom_read_dword(const uint32_t * ptr);

/**
 * Read 32 bit float from eeprom.
 */
float irqd_eeprom_read_float(const float * ptr);

/**
 * Read an entire block of size sz from src in eeprom to dst in sram.
 */
void irqd_eeprom_read_block(void * dst, void * src, uint8_t sz);

/**
 * Write byte value to address ptr in eeprom.
 *
 * To avoid unneccessary writes, use update instead!
 */
void irqd_eeprom_write_byte(uint8_t * ptr, uint8_t value);

/**
 * Write 16 bit word to address ptr in eeprom.
 *
 * To avoid unneccessary writes, use update instead!
 */
void irqd_eeprom_write_word(uint16_t * ptr, uint16_t value);

/**
 * Write 32 bit dword to address ptr in eeprom.
 *
 * To avoid unneccessary writes, use update instead!
 */
void irqd_eeprom_write_dword(uint32_t * ptr, uint32_t value);

/**
 * Write 32 bit float to address ptr in eeprom.
 *
 * To avoid unneccessary writes, use update instead!
 */
void irqd_eeprom_write_float(float * ptr, float value);

/**
 * Write data block of size sz from src in sram to dst in eeprom.
 *
 * To avoid unneccessary writes, use update instead!
 */
void irqd_eeprom_write_block(const void * src, void * dst, uint8_t sz);

/**
 * Update one byte in eeprom.
 */
void irqd_eeprom_update_byte(uint8_t * ptr, uint8_t value);

/**
 * Update one 16 bit word in eeprom.
 */
void irqd_eeprom_update_word(uint16_t * ptr, uint16_t value);

/**
 * Update one 32 bit word in eeprom.
 */
void irqd_eeprom_update_dword(uint32_t * ptr, uint32_t value);

/**
 * Update one 32 bit float in eeprom.
 */
void irqd_eeprom_update_float(float * ptr, float value);

/**
 * Update a block of size sz from src in sram to dst in eeprom.
 */
void irqd_eeprom_update_block(const void * src, void * dst, uint8_t sz);


#endif /* EEPROM_H_ */
