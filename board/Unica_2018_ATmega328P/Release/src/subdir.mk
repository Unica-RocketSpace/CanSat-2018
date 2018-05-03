################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/deployment_control.c \
../src/main.c \
../src/motor_control.c \
../src/nRF24L01.c \
../src/radio-module.c 

OBJS += \
./src/deployment_control.o \
./src/main.o \
./src/motor_control.o \
./src/nRF24L01.o \
./src/radio-module.o 

C_DEPS += \
./src/deployment_control.d \
./src/main.d \
./src/motor_control.d \
./src/nRF24L01.d \
./src/radio-module.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"/home/developer/git/Unica_2018_ATmega328P/librscs-master/rscs" -I"/home/developer/git/Unica_2018_ATmega328P/src" -I"/home/developer/git/Unica_2018_ATmega328P" -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


