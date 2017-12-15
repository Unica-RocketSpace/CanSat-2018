################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/drivers/GPS_NEO-6M.c \
../src/drivers/MPU9255.c \
../src/drivers/minmea.c 

OBJS += \
./src/drivers/GPS_NEO-6M.o \
./src/drivers/MPU9255.o \
./src/drivers/minmea.o 

C_DEPS += \
./src/drivers/GPS_NEO-6M.d \
./src/drivers/MPU9255.d \
./src/drivers/minmea.d 


# Each subdirectory must supply rules for building sources it contributes
src/drivers/%.o: ../src/drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/sofa" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src/drivers" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src/library" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src/modules" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/system/include/stm32f4-hal" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/include" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/system/include" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/system/include/cmsis" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


