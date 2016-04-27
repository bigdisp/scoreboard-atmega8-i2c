Main Atmega8 I2C Scoreboard Interface
=====================================

This project provides a simple i2c interface for controlling one connected seven segment numbers with an optional additional dot output on the 8th pin of the interface. It optionally provides PWM and, for testing purposes, can enable and disable a second row of LED if the second row is connected correctly.   
 
Requirements:
-------------
 * Atmega8 (other microcontrollers may be suitable if the code is adjusted accordingly (ports, clock, etc.),
 * A suitable flashing device,
 * A suitable i2c controller, such as the raspberry pi with `i2c-tools` installed or similar.

Example Schematics:
-------------------
TODO
        
Build:
------
Navigate to the debug or release folders and run `make`.

Usage:
------
The LED Output is assumed to be on port B. Port C is required for the i2c interface and contains the reset pin, leaving port D for an optional second output. Due to the choice of port B for the output, it is not possible to connect an external oscillator. The default i2c address of the atmega8 is `0x10`. 

Besides simple output commands, a few special commands are interpreted (all of these are subject to change and mainly present for debugging purposes, so be sure to recheck these before updating!):

 * The virtual 16 bit registers `0x00` and `0x01` control the on and off times of PWM. They default to 1000 and 3000, respectively. Units are clock cycles. Answers with the new pwm on and off values.
 * Writing anything to the virtual register `0x02` disables PWM.
 * Writing anything to the virtual register `0x03` reenables PWM. If the message written is `0x01` the pwm timings are set to 1000 on and 1000 off.
 * Writing `0x01` to the virtual register `0x04` will reset the pwm timings to 1000/1000. Otherwise, the old values stored are set as pwm timings again.
 * Writing anything to the virtual register `0x05` will set the pwm timings to 1000/1000. 
 * Writing anything to the virtual register `0x06` will set the pwm timings to 1000/2000.
 * Writing anything to the virtual register `0x07` will set the pwm timings to 1000/3000 (system default on start).
 * Values written to the virtual register `0x08` are stored as second row data, which controls the point on the seven segment display, but currently is connected to the second row of LED. Therefore, values that evaluate to boolean true will enable the second row of LED on the current prototype.
 * anything else written is interpreted as a symbol to be displayed. If a second row was previously stored, that will be used to decide whether the point should be displayed or not. 

License
-------
See [License.md](License.md).