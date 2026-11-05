/*
 * PC test suite for the scoreboard display firmware (atmega8/88p).
 *
 * anzeigeboard.c (7-segment driver) and main.c (I2C command dispatch) are
 * compiled unchanged for the host. AVR registers, I2C, PWM and EEPROM are
 * mocked (tests/mock), so no hardware is needed.
 */
#include <string.h>
#include "mock/avr_mock.h"
#include "anzeigeboard.h"
#include "minitest.h"

/* Internal functions of anzeigeboard.c (not in the header) */
uint8_t anzeige_convert(uint8_t ziffer);
void anzeige_einer(uint8_t bitmask);
void anzeige_zehner(uint8_t bitmask);

/* main() of main.c, renamed via -Dmain=firmware_main */
int firmware_main(void);

/* Software segment order (see uc/anzeigeboard.h): A..G = bit 0..6, PT = bit 7 */
enum { A = 1, B = 2, C = 4, D = 8, E = 16, F = 32, G = 64, PT = 128 };

/* Standard 7-segment encodings */
static const uint8_t DIGITS[10] = {
	A|B|C|D|E|F, B|C, A|B|D|E|G, A|B|C|D|G, B|C|F|G,
	A|C|D|F|G, A|C|D|E|F|G, A|B|C, A|B|C|D|E|F|G, A|B|C|D|F|G,
};

/*
 * Decode the segment pattern currently shown, straight from the port
 * registers. The pin mapping is written down independently of the firmware
 * headers so that a wiring change in uc/anzeigeboard.h is detected.
 */
static uint8_t shown_digit0(void)
{
	uint8_t m = 0;
	if (PORTB & (1 << 0)) m |= A;
	if (PORTB & (1 << 4)) m |= B;
	if (PORTB & (1 << 5)) m |= C;
	if (PORTB & (1 << 6)) m |= D;
	if (PORTB & (1 << 7)) m |= E;
	if (PORTC & (1 << 0)) m |= F;
	if (PORTC & (1 << 1)) m |= G;
	return m;
}

static uint8_t shown_digit1(void)
{
	uint8_t m = 0;
	if (PORTD & (1 << 0)) m |= A;
	if (PORTC & (1 << 3)) m |= B;
	if (PORTC & (1 << 2)) m |= C;
	if (PORTD & (1 << 2)) m |= D;
	if (PORTD & (1 << 1)) m |= E;
	if (PORTD & (1 << 7)) m |= F;
	if (PORTD & (1 << 4)) m |= G;
	return m;
}

/* Pins NOT used by the display: PWM outputs (PB1-3, PD3/5/6), TWI (PC4/PC5), reset (PC6) */
#define FOREIGN_B 0x0E
#define FOREIGN_C 0x70
#define FOREIGN_D 0x68

/* Run firmware main() until all queued I2C messages are consumed. */
static void run_firmware(void)
{
	if (!setjmp(mock_main_exit))
	{
		firmware_main();
	}
}

/* ======================================================================= */
/* 7-segment conversion                                                    */
/* ======================================================================= */

TEST_CASE(convert_digits_numeric_and_ascii)
{
	int i;
	for (i = 0; i < 10; ++i)
	{
		CHECK_EQ(DIGITS[i], anzeige_convert((uint8_t) i));
		CHECK_EQ(DIGITS[i], anzeige_convert((uint8_t) ('0' + i)));
	}
}

TEST_CASE(convert_letters)
{
	CHECK_EQ(A|B|C|E|F|G, anzeige_convert('A'));
	CHECK_EQ(C|D|E|F|G,   anzeige_convert('b'));
	CHECK_EQ(A|D|E|F,     anzeige_convert('C'));
	CHECK_EQ(B|C|D|E|G,   anzeige_convert('d'));
	CHECK_EQ(A|D|E|F|G,   anzeige_convert('E'));
	CHECK_EQ(A|E|F|G,     anzeige_convert('F'));
	CHECK_EQ(B|C|E|F|G,   anzeige_convert('H'));
	CHECK_EQ(D|E|F,       anzeige_convert('L'));
	CHECK_EQ(C|E|G,       anzeige_convert('n'));
	CHECK_EQ(C|D|E|G,     anzeige_convert('o'));
	CHECK_EQ(A|B|E|F|G,   anzeige_convert('P'));
	CHECK_EQ(E|G,         anzeige_convert('r'));
	CHECK_EQ(D|E|F|G,     anzeige_convert('t'));
	CHECK_EQ(B|C|D|F|G,   anzeige_convert('y'));
}

TEST_CASE(convert_case_insensitive)
{
	int c;
	for (c = 'a'; c <= 'z'; ++c)
	{
		CHECK_EQ(anzeige_convert((uint8_t) (c - 'a' + 'A')), anzeige_convert((uint8_t) c));
	}
}

TEST_CASE(convert_letters_never_fall_back_to_dash)
{
	int c;
	for (c = 'A'; c <= 'Z'; ++c)
	{
		CHECK(anzeige_convert((uint8_t) c) != G);
	}
}

TEST_CASE(convert_control_code_aliases)
{
	/* Codes 10..19 are short aliases for frequently used letters */
	CHECK_EQ(anzeige_convert('E'), anzeige_convert(10));
	CHECK_EQ(anzeige_convert('r'), anzeige_convert(11));
	CHECK_EQ(anzeige_convert('o'), anzeige_convert(12));
	CHECK_EQ(anzeige_convert('i'), anzeige_convert(13));
	CHECK_EQ(anzeige_convert('t'), anzeige_convert(14));
	CHECK_EQ(anzeige_convert('L'), anzeige_convert(15));
	CHECK_EQ(anzeige_convert('H'), anzeige_convert(16));
	CHECK_EQ(anzeige_convert('b'), anzeige_convert(17));
	CHECK_EQ(anzeige_convert('C'), anzeige_convert(18));
	CHECK_EQ(anzeige_convert('U'), anzeige_convert(19));
}

TEST_CASE(convert_symbols)
{
	CHECK_EQ(0,  anzeige_convert(' '));
	CHECK_EQ(G,  anzeige_convert('-'));
	CHECK_EQ(D,  anzeige_convert('_'));
	CHECK_EQ(D|G, anzeige_convert('='));
	CHECK_EQ(PT, anzeige_convert('.'));
	CHECK_EQ(PT, anzeige_convert(','));
	CHECK_EQ(PT, anzeige_convert(':'));
	CHECK_EQ(PT, anzeige_convert(';'));
	CHECK_EQ(A|C|D|E, anzeige_convert(0xFA)); /* firmware default/idle value */
}

TEST_CASE(convert_unknown_is_dash)
{
	CHECK_EQ(G, anzeige_convert('#'));
	CHECK_EQ(G, anzeige_convert('?'));
	CHECK_EQ(G, anzeige_convert(200));
	CHECK_EQ(G, anzeige_convert(255));
}

/* ======================================================================= */
/* Port mapping                                                            */
/* ======================================================================= */

TEST_CASE(init_configures_exactly_the_segment_pins_as_outputs)
{
	mock_reset();
	anzeige_init();
	CHECK_EQ(0xF1, DDRB); /* PB0, PB4-7 */
	CHECK_EQ(0x0F, DDRC); /* PC0-3 */
	CHECK_EQ(0x97, DDRD); /* PD0-2, PD4, PD7 */
	CHECK_EQ(0, shown_digit0());
}

TEST_CASE(every_segment_pattern_roundtrips_on_both_digits)
{
	int m;
	mock_reset();
	for (m = 0; m < 128; ++m)
	{
		anzeige_einer((uint8_t) m);
		anzeige_zehner((uint8_t) (127 - m));
		CHECK_EQ(m, shown_digit0());
		CHECK_EQ(127 - m, shown_digit1());
	}
}

TEST_CASE(digits_do_not_interfere)
{
	mock_reset();
	anzeige_zehner(A|B|C|D|E|F|G);
	anzeige_einer(0);
	CHECK_EQ(A|B|C|D|E|F|G, shown_digit1());
	anzeige_einer(A|B|C|D|E|F|G);
	anzeige_zehner(0);
	CHECK_EQ(A|B|C|D|E|F|G, shown_digit0());
	CHECK_EQ(0, shown_digit1());
}

TEST_CASE(display_never_touches_pwm_twi_or_reset_pins)
{
	int m;
	mock_reset();
	PORTB = FOREIGN_B; PORTC = FOREIGN_C; PORTD = FOREIGN_D;
	anzeige_init();
	for (m = 0; m < 256; ++m)
	{
		anzeige_write_convert((uint8_t) m, (uint8_t) (255 - m));
		anzeige_set_bits_0((uint8_t) m);
		anzeige_set_bits_1((uint8_t) m);
		anzeige_clear_bits_0((uint8_t) m);
		anzeige_clear_bits_1((uint8_t) m);
	}
	anzeige_write(88);
	CHECK_EQ(FOREIGN_B, PORTB & FOREIGN_B);
	CHECK_EQ(FOREIGN_C, PORTC & FOREIGN_C);
	CHECK_EQ(FOREIGN_D, PORTD & FOREIGN_D);
	CHECK_EQ(0, DDRB & FOREIGN_B);
	CHECK_EQ(0, DDRC & FOREIGN_C);
	CHECK_EQ(0, DDRD & FOREIGN_D);
}

/* ======================================================================= */
/* Display API                                                             */
/* ======================================================================= */

TEST_CASE(write_number_two_digits)
{
	int n;
	mock_reset();
	for (n = 10; n < 100; ++n)
	{
		anzeige_write((uint8_t) n);
		CHECK_EQ(DIGITS[n / 10], shown_digit1());
		CHECK_EQ(DIGITS[n % 10], shown_digit0());
	}
}

TEST_CASE(write_number_single_digit_blanks_tens)
{
	int n;
	mock_reset();
	for (n = 0; n < 10; ++n)
	{
		anzeige_write((uint8_t) n);
		CHECK_EQ(0, shown_digit1());
		CHECK_EQ(DIGITS[n], shown_digit0());
	}
}

TEST_CASE(write_number_above_99_shows_9_in_tens)
{
	mock_reset();
	anzeige_write(100);
	CHECK_EQ(DIGITS[9], shown_digit1());
	anzeige_write(255);
	CHECK_EQ(DIGITS[9], shown_digit1());
}

TEST_CASE(write_convert_both_digits)
{
	mock_reset();
	anzeige_write_convert('A', 'b');
	CHECK_EQ(anzeige_convert('A'), shown_digit0());
	CHECK_EQ(anzeige_convert('b'), shown_digit1());
	anzeige_write_convert_0(' ');
	CHECK_EQ(0, shown_digit0());
	CHECK_EQ(anzeige_convert('b'), shown_digit1());
	anzeige_write_convert_1('7');
	CHECK_EQ(DIGITS[7], shown_digit1());
}

TEST_CASE(write_direct_uses_software_bit_order)
{
	u_sevensegment_screen s;
	mock_reset();
	s.bitmask = 0;
	s.seg_a = 1; s.seg_d = 1; s.seg_g = 1;
	anzeige_write_direct(s);
	CHECK_EQ(A|D|G, shown_digit0());
}

TEST_CASE(set_and_clear_bits_leave_other_segments_alone)
{
	mock_reset();
	anzeige_write_convert('8', '8');
	anzeige_clear_bits_0(A|G);
	CHECK_EQ(B|C|D|E|F, shown_digit0());
	CHECK_EQ(DIGITS[8], shown_digit1());
	anzeige_clear_bits_1(D);
	CHECK_EQ(A|B|C|E|F|G, shown_digit1());
	anzeige_set_bits_0(G);
	CHECK_EQ(B|C|D|E|F|G, shown_digit0());
	anzeige_set_bits_1(D);
	CHECK_EQ(DIGITS[8], shown_digit1());
}

TEST_CASE(selftest_ends_with_blank_display)
{
	mock_reset();
	anzeige_init();
	anzeige_test();
	CHECK_EQ(0, shown_digit0());
	CHECK_EQ(0, shown_digit1());
}

/* ======================================================================= */
/* Firmware startup and I2C command protocol (main.c)                      */
/* ======================================================================= */

TEST_CASE(startup_defaults)
{
	mock_reset();
	run_firmware();
	CHECK_EQ(0x10, mock_i2c.address);           /* empty EEPROM -> default address */
	CHECK_EQ(1, mock_i2c.init_count);
	CHECK_EQ(1, mock_i2c.enabled);
	CHECK_EQ(1, mock_i2c.irq_enabled_at_enable); /* never ACK with interrupts off (bus lockup) */
	CHECK_EQ(3, mock_i2c.tx_size);
	CHECK_EQ(0xFA, mock_i2c.tx[0]);
	CHECK_EQ(1, mock_pwm.init_count);
	CHECK_EQ(1, mock_pwm.enabled);
	CHECK_EQ(200, mock_pwm.r[0]); CHECK_EQ(200, mock_pwm.b[0]); CHECK_EQ(511, mock_pwm.g[0]);
	CHECK_EQ(200, mock_pwm.r[1]); CHECK_EQ(200, mock_pwm.b[1]); CHECK_EQ(511, mock_pwm.g[1]);
	CHECK_EQ(1, mock_irq_enabled);
	CHECK_EQ(0, shown_digit0());
	CHECK_EQ(0, shown_digit1());
}

TEST_CASE(startup_address_from_eeprom)
{
	mock_reset();
	mock_eeprom[0] = 0x23;
	run_firmware();
	CHECK_EQ(0x23, mock_i2c.address);
	mock_reset();
	mock_eeprom[0] = 0x77; /* highest valid 7-bit address */
	run_firmware();
	CHECK_EQ(0x77, mock_i2c.address);
}

TEST_CASE(startup_rejects_reserved_eeprom_addresses)
{
	const uint8_t bad[] = { 0x00, 0x0F, 0x78, 0x7F, 0x80, 0xF0, 0xFF };
	unsigned i;
	for (i = 0; i < sizeof(bad); ++i)
	{
		mock_reset();
		mock_eeprom[0] = bad[i];
		run_firmware();
		CHECK_EQ(0x10, mock_i2c.address);
	}
}

TEST_CASE(cmd_06_writes_number)
{
	mock_reset();
	I2C_SEND(0x06, 42);
	run_firmware();
	CHECK_EQ(DIGITS[4], shown_digit1());
	CHECK_EQ(DIGITS[2], shown_digit0());
}

TEST_CASE(cmd_07_08_write_single_digits)
{
	mock_reset();
	I2C_SEND(0x07, 'A');
	I2C_SEND(0x08, 'b');
	run_firmware();
	CHECK_EQ(anzeige_convert('A'), shown_digit0());
	CHECK_EQ(anzeige_convert('b'), shown_digit1());

	mock_reset();
	I2C_SEND(0x08, 'H');
	I2C_SEND(0x07, '3');
	I2C_SEND(0x07, ' ');
	run_firmware();
	CHECK_EQ(0, shown_digit0());
	CHECK_EQ(anzeige_convert('H'), shown_digit1());
}

TEST_CASE(plain_ascii_writes_lower_digit_and_keeps_upper)
{
	mock_reset();
	I2C_SEND(0x08, 'H');
	I2C_SEND('5');
	run_firmware();
	CHECK_EQ(DIGITS[5], shown_digit0());
	CHECK_EQ(anzeige_convert('H'), shown_digit1());
}

TEST_CASE(cmd_09_to_0c_set_and_clear_bits)
{
	mock_reset();
	I2C_SEND(0x09, A|C);
	I2C_SEND(0x0B, B|D|F);
	run_firmware();
	CHECK_EQ(A|C, shown_digit0());
	CHECK_EQ(B|D|F, shown_digit1());

	mock_reset();
	I2C_SEND(0x09, A|C|E);
	I2C_SEND(0x0A, C);
	I2C_SEND(0x0B, G|F);
	I2C_SEND(0x0C, G);
	run_firmware();
	CHECK_EQ(A|E, shown_digit0());
	CHECK_EQ(F, shown_digit1());
}

TEST_CASE(cmd_00_sets_red_blue_segment0_and_replies_state)
{
	mock_reset();
	I2C_SEND(0x00, 0x12, 0x34);
	run_firmware();
	CHECK_EQ(0x12, mock_pwm.r[0]);
	CHECK_EQ(0x34, mock_pwm.b[0]);
	CHECK_EQ(200, mock_pwm.r[1]);
	/* Reply: r, b, g high, g low */
	CHECK_EQ(4, mock_i2c.tx_size);
	CHECK_EQ(0x12, mock_i2c.tx[0]);
	CHECK_EQ(0x34, mock_i2c.tx[1]);
}

TEST_CASE(cmd_01_sets_green_segment0_little_endian)
{
	mock_reset();
	I2C_SEND(0x01, 0xFF, 0x01); /* word 0x01FF, low byte first (i2cset "w") */
	run_firmware();
	CHECK_EQ(0x01FF, mock_pwm.g[0]);
	CHECK_EQ(511, mock_pwm.g[1]);
	CHECK_EQ(0x01, mock_i2c.tx[2]); /* reply is big endian */
	CHECK_EQ(0xFF, mock_i2c.tx[3]);
}

TEST_CASE(cmd_02_03_set_segment1)
{
	mock_reset();
	I2C_SEND(0x02, 0x56, 0x78);
	I2C_SEND(0x03, 0x34, 0x01);
	run_firmware();
	CHECK_EQ(0x56, mock_pwm.r[1]);
	CHECK_EQ(0x78, mock_pwm.b[1]);
	CHECK_EQ(0x0134, mock_pwm.g[1]);
	CHECK_EQ(200, mock_pwm.r[0]);
	CHECK_EQ(511, mock_pwm.g[0]);
	CHECK_EQ(0x56, mock_i2c.tx[0]);
	CHECK_EQ(0x78, mock_i2c.tx[1]);
	CHECK_EQ(0x01, mock_i2c.tx[2]);
	CHECK_EQ(0x34, mock_i2c.tx[3]);
}

TEST_CASE(cmd_04_05_disable_enable_pwm)
{
	mock_reset();
	I2C_SEND(0x04, 0x00);
	run_firmware();
	CHECK_EQ(0, mock_pwm.enabled);
	CHECK_EQ(0, mock_pwm.output_disabled_calls);

	mock_reset();
	I2C_SEND(0x04, 0x05);
	run_firmware();
	CHECK_EQ(0, mock_pwm.enabled);
	CHECK_EQ(1, mock_pwm.output_disabled_calls);
	CHECK_EQ(0x05, mock_pwm.output_disabled);

	mock_reset();
	I2C_SEND(0x04);
	I2C_SEND(0x05);
	run_firmware();
	CHECK_EQ(1, mock_pwm.enabled);
}

TEST_CASE(address_change_full_sequence_is_persisted)
{
	mock_reset();
	I2C_SEND(0x0D, 0x20, 0x20); /* 1: select new address (given twice) */
	I2C_SEND(0x0E, 0x20, 0x20); /* 2: switch to it temporarily */
	I2C_SEND(0x0F, 0x20, 0x10); /* 3: persist (new addr, old eeprom value) */
	run_firmware();
	CHECK_EQ(0x20, mock_i2c.address);
	CHECK_EQ(1, mock_i2c.init_count);
	CHECK_EQ(1, mock_i2c.set_address_count);
	CHECK_EQ(0x20, mock_eeprom[0]);
	CHECK_EQ(1, mock_i2c.eeprom_writes);
}

TEST_CASE(address_change_requires_matching_bytes)
{
	mock_reset();
	I2C_SEND(0x0D, 0x20, 0x21);
	I2C_SEND(0x0E, 0x20, 0x20);
	run_firmware();
	CHECK_EQ(0x10, mock_i2c.address);
	CHECK_EQ(0, mock_i2c.set_address_count);
}

TEST_CASE(address_change_rejects_reserved_addresses)
{
	const uint8_t bad[] = { 0x00, 0x0F, 0x78, 0x7F, 0x80, 0xF0, 0xFF };
	unsigned i;
	for (i = 0; i < sizeof(bad); ++i)
	{
		mock_reset();
		I2C_SEND(0x0D, bad[i], bad[i]);
		I2C_SEND(0x0E, bad[i], bad[i]);
		run_firmware();
		CHECK_EQ(0x10, mock_i2c.address);
		CHECK_EQ(0, mock_i2c.set_address_count);
	}
}

TEST_CASE(address_persist_requires_temporary_switch_first)
{
	mock_reset();
	I2C_SEND(0x0D, 0x20, 0x20);
	I2C_SEND(0x0F, 0x20, 0x10); /* step 2 skipped */
	run_firmware();
	CHECK_EQ(0x10, mock_i2c.address);
	CHECK_EQ(0xFF, mock_eeprom[0]);
	CHECK_EQ(0, mock_i2c.eeprom_writes);
}

TEST_CASE(address_persist_requires_current_eeprom_value)
{
	mock_reset();
	I2C_SEND(0x0D, 0x20, 0x20);
	I2C_SEND(0x0E, 0x20, 0x20);
	I2C_SEND(0x0F, 0x20, 0x33); /* wrong old value */
	run_firmware();
	CHECK_EQ(0xFF, mock_eeprom[0]);
}

/* Same bytes as fbl_flash.py of atmega_i2c_fbl sends ("i2cset ... 0x10 0xFB4C w") */
TEST_CASE(cmd_enter_bootloader_resets_via_watchdog)
{
	mock_reset();
	I2C_SEND(0x07, 'A');
	I2C_SEND(0x10, 0x4C, 0xFB);
	I2C_SEND(0x07, 'B'); /* never processed */
	run_firmware();
	CHECK_EQ(1, mock_i2c.wdt_reset);
	CHECK_EQ(0, mock_i2c.irq_enabled_at_wdt_reset);
	CHECK_EQ(0, mock_i2c.twcr_at_wdt_reset); /* TWI off: no clock stretching until reset */
	CHECK_EQ(anzeige_convert('A'), shown_digit0());
	CHECK_EQ(2, mock_i2c.rx_pos);
}

TEST_CASE(enter_bootloader_needs_exact_three_bytes)
{
	mock_reset();
	I2C_SEND(0x10);                   /* plain 0x10 is the symbol 'H' */
	I2C_SEND(0x10, 0x4C);
	I2C_SEND(0x10, 0x4C, 0xFB, 0x00); /* 4 bytes */
	I2C_SEND(0x10, 0x4C, 0xFA);
	run_firmware();
	CHECK_EQ(0, mock_i2c.wdt_reset);
	CHECK_EQ(anzeige_convert('H'), shown_digit0());
}

TEST_CASE(reply_mirrors_last_command)
{
	mock_reset();
	I2C_SEND('7');
	run_firmware();
	CHECK_EQ(4, mock_i2c.tx_size);
	CHECK_EQ('7', mock_i2c.tx[0]);
}

int main(void)
{
	printf("== 7-segment conversion\n");
	RUN(convert_digits_numeric_and_ascii);
	RUN(convert_letters);
	RUN(convert_case_insensitive);
	RUN(convert_letters_never_fall_back_to_dash);
	RUN(convert_control_code_aliases);
	RUN(convert_symbols);
	RUN(convert_unknown_is_dash);

	printf("== port mapping\n");
	RUN(init_configures_exactly_the_segment_pins_as_outputs);
	RUN(every_segment_pattern_roundtrips_on_both_digits);
	RUN(digits_do_not_interfere);
	RUN(display_never_touches_pwm_twi_or_reset_pins);

	printf("== display API\n");
	RUN(write_number_two_digits);
	RUN(write_number_single_digit_blanks_tens);
	RUN(write_number_above_99_shows_9_in_tens);
	RUN(write_convert_both_digits);
	RUN(write_direct_uses_software_bit_order);
	RUN(set_and_clear_bits_leave_other_segments_alone);
	RUN(selftest_ends_with_blank_display);

	printf("== firmware startup / I2C protocol\n");
	RUN(startup_defaults);
	RUN(startup_address_from_eeprom);
	RUN(startup_rejects_reserved_eeprom_addresses);
	RUN(cmd_06_writes_number);
	RUN(cmd_07_08_write_single_digits);
	RUN(plain_ascii_writes_lower_digit_and_keeps_upper);
	RUN(cmd_09_to_0c_set_and_clear_bits);
	RUN(cmd_00_sets_red_blue_segment0_and_replies_state);
	RUN(cmd_01_sets_green_segment0_little_endian);
	RUN(cmd_02_03_set_segment1);
	RUN(cmd_04_05_disable_enable_pwm);
	RUN(address_change_full_sequence_is_persisted);
	RUN(address_change_requires_matching_bytes);
	RUN(address_change_rejects_reserved_addresses);
	RUN(address_persist_requires_temporary_switch_first);
	RUN(address_persist_requires_current_eeprom_value);

	RUN(cmd_enter_bootloader_resets_via_watchdog);
	RUN(enter_bootloader_needs_exact_three_bytes);
	RUN(reply_mirrors_last_command);

	return minitest_summary();
}
