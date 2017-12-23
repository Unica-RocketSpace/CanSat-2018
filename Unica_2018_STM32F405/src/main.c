
#include <stdio.h>
#include <stdlib.h>

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_cortex.h>
#include <stm32f4xx_hal_i2c.h>
#include <stm32f4xx_hal_usart.h>
#include <stm32f4xx_hal_dma.h>
#include <stm32f4xx_hal_gpio.h>

#include <FreeRTOS.h>
#include "task.h"

#include "diag/Trace.h"

#include "Timer.h"
#include "BlinkLed.h"

#include "state.h"
#include "kinematic_unit.h"
#include "gps_nmea.h"

// ----- Timing definitions -------------------------------------------------

// Keep the LED on for 2/3 of a second.
#define BLINK_ON_TICKS  (TIMER_FREQUENCY_HZ * 3 / 4)
#define BLINK_OFF_TICKS (TIMER_FREQUENCY_HZ - BLINK_ON_TICKS)

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


#define GPS_TASK_STACK_SIZE 1024
static StackType_t _gpsTaskStack[GPS_TASK_STACK_SIZE];
static StaticTask_t _gpsTaskObj;


int main(int argc, char* argv[])
{
	// Инициализация системы
	HAL_Init();

	// Инициализация структур глобального состояния (в нашем случае просто заполняем их нулями)
	stateInit(globalState);
	stateInit(globalState_prev);

	// TODO:	РЕШИТЬ ПРОБЛЕМУ С FREERTOS
	// TODO:	ПОСМОТРЕТЬ USART+DMA

	xTaskCreateStatic(
			GPS_task, // функция
			"GPS", // имя
			GPS_TASK_STACK_SIZE, // глубина стенка
			NULL, // аргумент
			1, // приоритет
			_gpsTaskStack, // стек
			&_gpsTaskObj // объект задания
	);

	vTaskStartScheduler();

	return 0;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
