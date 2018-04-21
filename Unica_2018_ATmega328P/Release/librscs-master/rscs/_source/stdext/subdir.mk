################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../librscs-master/rscs/_source/stdext/stdio.c 

OBJS += \
./librscs-master/rscs/_source/stdext/stdio.o 

C_DEPS += \
./librscs-master/rscs/_source/stdext/stdio.d 


# Each subdirectory must supply rules for building sources it contributes
librscs-master/rscs/_source/stdext/%.o: ../librscs-master/rscs/_source/stdext/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"/home/developer/workspace-avr/Unica_2018_ATmega328P/librscs-master/rscs" -I"/home/developer/workspace-avr/Unica_2018_ATmega328P/src" -I"/home/developer/workspace-avr/Unica_2018_ATmega328P" -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


