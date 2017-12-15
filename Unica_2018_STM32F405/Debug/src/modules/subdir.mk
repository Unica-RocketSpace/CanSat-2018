################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/modules/dynamic_unit.c \
../src/modules/kinematic_unit.c \
../src/modules/telemetry.c 

OBJS += \
./src/modules/dynamic_unit.o \
./src/modules/kinematic_unit.o \
./src/modules/telemetry.o 

C_DEPS += \
./src/modules/dynamic_unit.d \
./src/modules/kinematic_unit.d \
./src/modules/telemetry.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/%.o: ../src/modules/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/sofa" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src/drivers" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src/library" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src/modules" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/system/include/stm32f4-hal" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/include" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/system/include" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/system/include/cmsis" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


