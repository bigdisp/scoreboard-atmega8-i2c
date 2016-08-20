Main Atmega8 I2C Scoreboard Interface
=====================================

This project provides a simple i2c interface for controlling one connected seven segment numbers with an optional additional dot output on the 8th pin of the interface. It optionally provides PWM and, for testing purposes, can enable and disable a second row of LED if the second row is connected correctly.   
 
Requirements:
-------------
 * Atmega88p(a) (other microcontrollers may be suitable if the code is adjusted accordingly (ports, clock, etc.),
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
The default i2c address of the atmega88p is `0x10`. The device takes either no parameters with the command, 8 bits of data or 16 bits of data. The software can control two digits individually or use them combined to display numbers between 0 and 99.

Anything that cannot be interpreted as a command is interpreted as an ascii symbol to be output on the primary port. For example, sending `0x32` will print a 2 on digit 0. A non-printable symbol will be replaced by a dash (-).


 * The virtual 16 bit registers `0x00` and `0x01` control the first PWM port. They take one 16 bit parameter. The upper byte of the parameter for `0x00` controls the pwm timing for the red component, the lower byte controls the blue component. `0x01` controls the green component. Values up to `0x01FF` are acceptable, thus the green component has twice the pwm precision as the other two colors. 
 * The second PWM port is likewise controlled by `0x02` (rb) and `0x03` (g).
 * Writing to the virtual register `0x04` disables PWM. The parameter given controls what colors on what port are enabled. Bit order is xxRrGgBb, where capital letters control the first, small letters control the second pwm port. Thus to enable port 0 with white light and disable port two completely, xx101010 could be used.
 * Writing anything to the virtual register `0x05` reenables pwm.
 * A byte written to the virtual register `0x06` is interpreted as a number and written to both digits.
 * A byte written to the virtual register `0x07` is written as ascii to the first output (right).
 * A byte written to the virtual register `0x08` is written as ascii to the second output (left).
 * The registers `0x09` and `0x0B` can be used to set individual bits on the first and second port respectively. Default bitorder for the parameter is GFEDCBA for both ports.
 * The registers `0x0A` and `0x0C` can be used to clear individual bits on the first and second port, respectively. Default bitorder for the parameter is GFEDCBA.
 * The Software supports a change of the i2c Address via i2c. Since this is dangerous to do in a live system, it requires a series of 2 commands for a temporary address change and a third command to write the new address to the eeprom. Please make sure that your address is valid (0x0F < address < 0xF0), otherwise your commands will be ignored. 
    1. Write the new address two times simultaneously to `0x0D`. For example, if the new address should be `0x12`, write `0x1212` to `0x0D`.
    2. Confirm the new address by writing it again two times to `0x0E`. This will cause an immediate change of the i2c address. The device will now respond to the new address, but fall back to the old address if it is reset.
    3. Write the address to the eeprom by writing the new address and the old address to `0x0F`. The old address must be the one currently stored in the eeprom. This defaults to `0x10` if no address was programmed before. For the example above, if the default address is still present, write `0x1210` to `0x0F`.
 * anything else written is interpreted as a symbol to be displayed.

Reading from the device with a generic read command will return the currently displayed digit.

License
-------
See [License.md](License.md).