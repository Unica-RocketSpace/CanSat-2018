################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/stm32f4-hal/stm32f4xx_hal.c \
../system/src/stm32f4-hal/stm32f4xx_hal_cortex.c \
../system/src/stm32f4-hal/stm32f4xx_hal_dfsdm.c \
../system/src/stm32f4-hal/stm32f4xx_hal_dma.c \
../system/src/stm32f4-hal/stm32f4xx_hal_dma_ex.c \
../system/src/stm32f4-hal/stm32f4xx_hal_flash.c \
../system/src/stm32f4-hal/stm32f4xx_hal_gpio.c \
../system/src/stm32f4-hal/stm32f4xx_hal_i2c.c \
../system/src/stm32f4-hal/stm32f4xx_hal_i2c_ex.c \
../system/src/stm32f4-hal/stm32f4xx_hal_iwdg.c \
../system/src/stm32f4-hal/stm32f4xx_hal_pwr.c \
../system/src/stm32f4-hal/stm32f4xx_hal_rcc.c \
../system/src/stm32f4-hal/stm32f4xx_hal_tim.c \
../system/src/stm32f4-hal/stm32f4xx_hal_tim_ex.c \
../system/src/stm32f4-hal/stm32f4xx_hal_uart.c \
../system/src/stm32f4-hal/stm32f4xx_hal_usart.c 

OBJS += \
./system/src/stm32f4-hal/stm32f4xx_hal.o \
./system/src/stm32f4-hal/stm32f4xx_hal_cortex.o \
./system/src/stm32f4-hal/stm32f4xx_hal_dfsdm.o \
./system/src/stm32f4-hal/stm32f4xx_hal_dma.o \
./system/src/stm32f4-hal/stm32f4xx_hal_dma_ex.o \
./system/src/stm32f4-hal/stm32f4xx_hal_flash.o \
./system/src/stm32f4-hal/stm32f4xx_hal_gpio.o \
./system/src/stm32f4-hal/stm32f4xx_hal_i2c.o \
./system/src/stm32f4-hal/stm32f4xx_hal_i2c_ex.o \
./system/src/stm32f4-hal/stm32f4xx_hal_iwdg.o \
./system/src/stm32f4-hal/stm32f4xx_hal_pwr.o \
./system/src/stm32f4-hal/stm32f4xx_hal_rcc.o \
./system/src/stm32f4-hal/stm32f4xx_hal_tim.o \
./system/src/stm32f4-hal/stm32f4xx_hal_tim_ex.o \
./system/src/stm32f4-hal/stm32f4xx_hal_uart.o \
./system/src/stm32f4-hal/stm32f4xx_hal_usart.o 

C_DEPS += \
./system/src/stm32f4-hal/stm32f4xx_hal.d \
./system/src/stm32f4-hal/stm32f4xx_hal_cortex.d \
./system/src/stm32f4-hal/stm32f4xx_hal_dfsdm.d \
./system/src/stm32f4-hal/stm32f4xx_hal_dma.d \
./system/src/stm32f4-hal/stm32f4xx_hal_dma_ex.d \
./system/src/stm32f4-hal/stm32f4xx_hal_flash.d \
./system/src/stm32f4-hal/stm32f4xx_hal_gpio.d \
./system/src/stm32f4-hal/stm32f4xx_hal_i2c.d \
./system/src/stm32f4-hal/stm32f4xx_hal_i2c_ex.d \
./system/src/stm32f4-hal/stm32f4xx_hal_iwdg.d \
./system/src/stm32f4-hal/stm32f4xx_hal_pwr.d \
./system/src/stm32f4-hal/stm32f4xx_hal_rcc.d \
./system/src/stm32f4-hal/stm32f4xx_hal_tim.d \
./system/src/stm32f4-hal/stm32f4xx_hal_tim_ex.d \
./system/src/stm32f4-hal/stm32f4xx_hal_uart.d \
./system/src/stm32f4-hal/stm32f4xx_hal_usart.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/stm32f4-hal/%.o: ../system/src/stm32f4-hal/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -march=armv7e-m -mthumb -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"/home/developer/git/Unica_2018_STM32F405/system/include" -I"/home/developer/git/Unica_2018_STM32F405/system/include/cmsis" -I"/home/developer/git/Unica_2018_STM32F405/system/include/stm32f4-hal" -I"/home/developer/git/Unica_2018_STM32F405/system-usr" -I"/home/developer/git/Unica_2018_STM32F405/FreeRTOS/include" -I"/home/developer/git/Unica_2018_STM32F405/FreeRTOS/portable" -I"/home/developer/git/Unica_2018_STM32F405/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/developer/git/Unica_2018_STM32F405/FreeRTOS-usr" -I"/home/developer/git/Unica_2018_STM32F405/sofa" -I"/home/developer/git/Unica_2018_STM32F405/src" -I"/home/developer/git/Unica_2018_STM32F405/src/drivers" -I"/home/developer/git/Unica_2018_STM32F405/src/library" -I"/home/developer/git/Unica_2018_STM32F405/src/modules" -std=gnu11 -Wno-bad-function-cast -Wno-conversion -Wno-sign-conversion -Wno-unused-parameter -Wno-sign-compare -Wno-missing-prototypes -Wno-missing-declarations -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


