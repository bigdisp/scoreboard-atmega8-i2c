/*
 * Host mocks for the AVR registers, EEPROM, I2C (TWI) driver and PWM driver.
 *
 * The real firmware modules anzeigeboard.c and main.c are compiled unchanged
 * against these mocks. I2C traffic is scripted: tests queue messages that the
 * "master" sends, run the firmware main loop and inspect what happened.
 */
#include "avr_mock.h"
#include <string.h>

volatile uint8_t PORTB, PORTC, PORTD;
volatile uint8_t DDRB, DDRC, DDRD;
volatile uint8_t TWAR, TWCR;
volatile uint8_t mock_MCUSR;
int mock_irq_enabled;
uint8_t mock_eeprom[MOCK_EEPROM_SIZE];

struct mock_i2c mock_i2c;
struct mock_pwm mock_pwm;
jmp_buf mock_main_exit;

void mock_reset(void)
{
	MCUSR = 0;
	PORTB = PORTC = PORTD = 0;
	DDRB = DDRC = DDRD = 0;
	TWAR = 0;
	TWCR = 0;
	mock_irq_enabled = 0;
	memset(mock_eeprom, 0xFF, sizeof(mock_eeprom));
	memset(&mock_i2c, 0, sizeof(mock_i2c));
	memset(&mock_pwm, 0, sizeof(mock_pwm));
}

void mock_i2c_send(const uint8_t *msg, int len)
{
	int i;
	for (i = 0; i < len && i < I2C_BUFFER_SIZE; ++i)
	{
		mock_i2c.rx_queue[mock_i2c.rx_count][i] = msg[i];
	}
	mock_i2c.rx_len[mock_i2c.rx_count++] = (uint8_t) len;
}

void mock_wdt_enable(int timeout)
{
	(void) timeout;
	mock_i2c.wdt_reset = 1;
	mock_i2c.twcr_at_wdt_reset = TWCR;
	mock_i2c.irq_enabled_at_wdt_reset = mock_irq_enabled;
	longjmp(mock_main_exit, 2);
}

/* ---- EEPROM ---------------------------------------------------------- */

uint8_t eeprom_read_byte(const uint8_t *addr)
{
	return mock_eeprom[(uintptr_t) addr % MOCK_EEPROM_SIZE];
}

void eeprom_update_byte(uint8_t *addr, uint8_t value)
{
	mock_eeprom[(uintptr_t) addr % MOCK_EEPROM_SIZE] = value;
}

void irq_ctrl(uint8_t enable)
{
	mock_irq_enabled = enable;
}

uint8_t irqd_eeprom_read_byte(const uint8_t *ptr)
{
	return eeprom_read_byte(ptr);
}

void irqd_eeprom_update_byte(uint8_t *ptr, uint8_t value)
{
	mock_eeprom[(uintptr_t) ptr % MOCK_EEPROM_SIZE] = value;
	mock_i2c.eeprom_writes++;
}

/* ---- I2C ------------------------------------------------------------- */

void i2c_init(uint8_t address)
{
	mock_i2c.init_count++;
	mock_i2c.address = address;
	TWAR = (uint8_t) (address << 1);
}

void i2c_slave_enable(void)
{
	mock_i2c.enabled = 1;
	mock_i2c.irq_enabled_at_enable = mock_irq_enabled;
	TWCR = 0x45; /* TWEA | TWEN | TWIE: slave armed */
}

void i2c_set_address(uint8_t address)
{
	mock_i2c.set_address_count++;
	mock_i2c.address = address;
	TWAR = (uint8_t) (address << 1);
}

uint8_t i2c_get_address(void)
{
	return mock_i2c.address;
}

void i2c_slave_set_tx_data(const uint8_t *msg, uint8_t size)
{
	uint8_t i;
	mock_i2c.tx_size = size;
	for (i = 0; i < size && i < I2C_BUFFER_SIZE; ++i)
	{
		mock_i2c.tx[i] = msg[i];
	}
}

/* Polled by the main loop: once the scripted messages are used up, leave
 * the (endless) firmware main loop. */
uint8_t i2c_get_message(uint8_t *msg, uint8_t size)
{
	uint8_t i, len;
	if (mock_i2c.rx_pos >= mock_i2c.rx_count)
	{
		longjmp(mock_main_exit, 1);
	}
	len = mock_i2c.rx_len[mock_i2c.rx_pos];
	for (i = 0; i < size; ++i)
	{
		msg[i] = i < len ? mock_i2c.rx_queue[mock_i2c.rx_pos][i] : 0;
	}
	mock_i2c.rx_pos++;
	return len;
}

void i2c_supervise(uint16_t elapsed_us) { (void) elapsed_us; }

/* ---- PWM ------------------------------------------------------------- */

void pwm_init(void)                 { mock_pwm.init_count++; }
void pwm_enable(uint8_t enable)     { mock_pwm.enabled = enable; mock_pwm.enable_calls++; }
void pwm_set_output_disabled(uint8_t rgb) { mock_pwm.output_disabled = rgb; mock_pwm.output_disabled_calls++; }
void pwm_set_timing_16(uint16_t active) { (void) active; }
void pwm_set_timing_8(uint8_t active)   { (void) active; }
void pwm_set_timing_g(uint16_t active, uint8_t port) { mock_pwm.g[port & 1] = active; }
void pwm_set_timing_b(uint8_t active, uint8_t port)  { mock_pwm.b[port & 1] = active; }
void pwm_set_timing_r(uint8_t active, uint8_t port)  { mock_pwm.r[port & 1] = active; }
