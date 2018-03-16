################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/cmsis/system_stm32f4xx.c \
../system/src/cmsis/vectors_stm32f405xx.c 

OBJS += \
./system/src/cmsis/system_stm32f4xx.o \
./system/src/cmsis/vectors_stm32f405xx.o 

C_DEPS += \
./system/src/cmsis/system_stm32f4xx.d \
./system/src/cmsis/vectors_stm32f405xx.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/cmsis/%.o: ../system/src/cmsis/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -march=armv7e-m -mthumb -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"/home/developer/git/Unica_2018_STM32F405/system/include" -I"/home/developer/git/Unica_2018_STM32F405/system/include/cmsis" -I"/home/developer/git/Unica_2018_STM32F405/system/include/stm32f4-hal" -I"/home/developer/git/Unica_2018_STM32F405/system-usr" -I"/home/developer/git/Unica_2018_STM32F405/FreeRTOS/include" -I"/home/developer/git/Unica_2018_STM32F405/FreeRTOS/portable" -I"/home/developer/git/Unica_2018_STM32F405/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/developer/git/Unica_2018_STM32F405/FreeRTOS-usr" -I"/home/developer/git/Unica_2018_STM32F405/sofa" -I"/home/developer/git/Unica_2018_STM32F405/src" -I"/home/developer/git/Unica_2018_STM32F405/src/drivers" -I"/home/developer/git/Unica_2018_STM32F405/src/library" -I"/home/developer/git/Unica_2018_STM32F405/src/modules" -I"/home/developer/mavlink-messages" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


