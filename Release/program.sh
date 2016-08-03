#!/bin/bash

avrdude -B 10kHz -i 2 -P usb -c avrisp2 -p m88p -U flash:w:rgn_scoreboard_atmega8_i2c.hex:i
