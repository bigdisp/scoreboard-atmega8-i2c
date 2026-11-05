# PC test suite for the scoreboard display firmware.
# Compiles src/anzeigeboard.c and src/main.c for the host against mocked AVR
# registers / drivers (tests/mock). No microcontroller or programmer required.
#
#   make -C tests          # build + run unit tests
#   make -C tests avr      # check the firmware builds with avr-gcc and leaves room for the bootloader
#   make -C tests clean

CC      ?= gcc
CFLAGS  ?= -O0 -g -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-attributes
CFLAGS  += -std=gnu99 -funsigned-char -DTEST -DF_CPU=8000000UL
CFLAGS  += -fsanitize=address,undefined -fno-omit-frame-pointer
INC     := -Imock -I../src
BUILD   := build
BIN     := $(BUILD)/test_firmware
SRCS    := test_firmware.c mock/avr_mock.c ../src/anzeigeboard.c ../src/main.c

.PHONY: all test avr clean
all: test

$(BUILD)/main.o: ../src/main.c $(wildcard ../src/*.h) $(wildcard mock/*.h mock/*/*.h)
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $(INC) -Dmain=firmware_main -c -o $@ $<

$(BIN): test_firmware.c mock/avr_mock.c ../src/anzeigeboard.c $(BUILD)/main.o minitest.h $(wildcard ../src/*.h ../src/*/*.h mock/*.h mock/*/*.h)
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $(INC) -o $@ test_firmware.c mock/avr_mock.c ../src/anzeigeboard.c $(BUILD)/main.o

test: $(BIN)
	./$(BIN)

# Application area below the I2C flash bootloader (atmega_i2c_fbl starts at 0x1800)
APP_MAX_BYTES := 6144

# Build the real firmware with avr-gcc (needs gcc-avr + avr-libc).
avr:
	$(MAKE) -C ../Release clean
	$(MAKE) -C ../Release all
	@size=$$(avr-size -A ../Release/rgn_scoreboard_atmega8_i2c.elf | awk '$$1==".text"||$$1==".data"{s+=$$2} END{print s}'); \
	echo "Firmware flash size: $$size bytes (limit below bootloader: $(APP_MAX_BYTES) bytes)"; \
	test $$size -le $(APP_MAX_BYTES) || { echo "ERROR: firmware would overlap the bootloader"; exit 1; }

clean:
	rm -rf $(BUILD)
