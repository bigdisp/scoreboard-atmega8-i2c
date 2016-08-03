################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
rgn_scoreboard_atmega8_i2c.lss \

SIZEDUMMY += \
sizedummy \


# All Target
all: rgn_scoreboard_atmega8_i2c.elf secondary-outputs

# Tool invocations
rgn_scoreboard_atmega8_i2c.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C++ Linker'
	avr-g++ -Wl,-Map,rgn_scoreboard_atmega8_i2c.map,--cref -mrelax -Wl,--gc-sections -mmcu=atmega88p -o "rgn_scoreboard_atmega8_i2c.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

rgn_scoreboard_atmega8_i2c.lss: rgn_scoreboard_atmega8_i2c.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S rgn_scoreboard_atmega8_i2c.elf  >"rgn_scoreboard_atmega8_i2c.lss"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: rgn_scoreboard_atmega8_i2c.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega88p rgn_scoreboard_atmega8_i2c.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(S_DEPS)$(LSS)$(C_DEPS)$(CC_DEPS)$(ELFS)$(C++_DEPS)$(OBJS)$(CXX_DEPS)$(ASM_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(CPP_DEPS) rgn_scoreboard_atmega8_i2c.elf
	-@echo ' '

secondary-outputs: $(LSS) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
