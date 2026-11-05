/* Host mock of <avr/eeprom.h>: EEPROM is the array mock_eeprom[] (512 bytes). */
#ifndef MOCK_AVR_EEPROM_H
#define MOCK_AVR_EEPROM_H

#include <stdint.h>

#define MOCK_EEPROM_SIZE 512
extern uint8_t mock_eeprom[MOCK_EEPROM_SIZE];

uint8_t eeprom_read_byte(const uint8_t *addr);
void eeprom_update_byte(uint8_t *addr, uint8_t value);

#endif
