################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BlinkLed.c \
../src/Timer.c \
../src/_initialize_hardware.c \
../src/_write.c \
../src/initialize_hardware.c \
../src/main.c \
../src/state.c \
../src/stm32f4xx_hal_msp.c 

OBJS += \
./src/BlinkLed.o \
./src/Timer.o \
./src/_initialize_hardware.o \
./src/_write.o \
./src/initialize_hardware.o \
./src/main.o \
./src/state.o \
./src/stm32f4xx_hal_msp.o 

C_DEPS += \
./src/BlinkLed.d \
./src/Timer.d \
./src/_initialize_hardware.d \
./src/_write.d \
./src/initialize_hardware.d \
./src/main.d \
./src/state.d \
./src/stm32f4xx_hal_msp.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/sofa" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src/drivers" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src/library" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src/modules" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/system/include/stm32f4-hal" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/include" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/system/include" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/system/include/cmsis" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/initialize_hardware.o: ../src/initialize_hardware.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/sofa" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src/drivers" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src/library" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src/modules" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/system/include/stm32f4-hal" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/include" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/system/include" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/system/include/cmsis" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/initialize_hardware.d" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/stm32f4xx_hal_msp.o: ../src/stm32f4xx_hal_msp.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/sofa" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src/drivers" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src/library" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/src/modules" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/system/include/stm32f4-hal" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/include" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/system/include" -I"/home/developer/workspace-stm32/Unica_2018_STM32F405/system/include/cmsis" -std=gnu11 -Wno-missing-prototypes -Wno-missing-declarations -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/stm32f4xx_hal_msp.d" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


