################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../librscs-master/rscs/_source/bmp280.c \
../librscs-master/rscs/_source/crc.c \
../librscs-master/rscs/_source/i2c.c \
../librscs-master/rscs/_source/ringbuf.c \
../librscs-master/rscs/_source/spi.c \
../librscs-master/rscs/_source/timeservice.c \
../librscs-master/rscs/_source/uart.c 

OBJS += \
./librscs-master/rscs/_source/bmp280.o \
./librscs-master/rscs/_source/crc.o \
./librscs-master/rscs/_source/i2c.o \
./librscs-master/rscs/_source/ringbuf.o \
./librscs-master/rscs/_source/spi.o \
./librscs-master/rscs/_source/timeservice.o \
./librscs-master/rscs/_source/uart.o 

C_DEPS += \
./librscs-master/rscs/_source/bmp280.d \
./librscs-master/rscs/_source/crc.d \
./librscs-master/rscs/_source/i2c.d \
./librscs-master/rscs/_source/ringbuf.d \
./librscs-master/rscs/_source/spi.d \
./librscs-master/rscs/_source/timeservice.d \
./librscs-master/rscs/_source/uart.d 


# Each subdirectory must supply rules for building sources it contributes
librscs-master/rscs/_source/%.o: ../librscs-master/rscs/_source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"/home/developer/git/Unica_2018_ATmega328P/librscs-master/rscs" -I"/home/developer/git/Unica_2018_ATmega328P/src" -I"/home/developer/git/Unica_2018_ATmega328P" -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


