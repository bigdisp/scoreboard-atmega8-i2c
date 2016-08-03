################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/anzeigeboard.c \
../src/eeprom.c \
../src/i2c.c \
../src/interrupt.c \
../src/main.c \
../src/pwm.c 

C_DEPS += \
./src/anzeigeboard.d \
./src/eeprom.d \
./src/i2c.d \
./src/interrupt.d \
./src/main.d \
./src/pwm.d 

OBJS += \
./src/anzeigeboard.o \
./src/eeprom.o \
./src/i2c.o \
./src/interrupt.o \
./src/main.o \
./src/pwm.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega88p -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


