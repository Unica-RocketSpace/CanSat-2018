################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include librscs-master/rscs/_source/stdext/subdir.mk
-include librscs-master/rscs/_source/subdir.mk
-include librscs-master/rscs/3rd_party/minmea/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
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
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
Unica_2018_ATmega328P.lss \

FLASH_IMAGE += \
Unica_2018_ATmega328P.hex \

EEPROM_IMAGE += \
Unica_2018_ATmega328P.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: Unica_2018_ATmega328P.elf secondary-outputs

# Tool invocations
Unica_2018_ATmega328P.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,Unica_2018_ATmega328P.map -Wl,-u,vfprintf -lprintf_flt -lm -mmcu=atmega328p -o "Unica_2018_ATmega328P.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

Unica_2018_ATmega328P.lss: Unica_2018_ATmega328P.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S Unica_2018_ATmega328P.elf  >"Unica_2018_ATmega328P.lss"
	@echo 'Finished building: $@'
	@echo ' '

Unica_2018_ATmega328P.hex: Unica_2018_ATmega328P.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Unica_2018_ATmega328P.elf  "Unica_2018_ATmega328P.hex"
	@echo 'Finished building: $@'
	@echo ' '

Unica_2018_ATmega328P.eep: Unica_2018_ATmega328P.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex Unica_2018_ATmega328P.elf  "Unica_2018_ATmega328P.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: Unica_2018_ATmega328P.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega328p Unica_2018_ATmega328P.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(EEPROM_IMAGE)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) Unica_2018_ATmega328P.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
