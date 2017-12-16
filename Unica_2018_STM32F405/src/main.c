
#include <stdio.h>
#include <stdlib.h>

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_cortex.h>
#include <stm32f4xx_hal_i2c.h>
#include <stm32f4xx_hal_usart.h>
#include <stm32f4xx_hal_dma.h>
#include <stm32f4xx_hal_gpio.h>

#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include "task.h"

#include "diag/Trace.h"

#include "Timer.h"
#include "BlinkLed.h"

#include "initialize_hardware.h"
#include "state.h"

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


state_t* state = {};
state_t* state_prev = {};

I2C_HandleTypeDef* i2c = {};
USART_HandleTypeDef* usart = {};
DMA_HandleTypeDef* dma = {};

uint8_t dma_usartBuffer[100];



int main(int argc, char* argv[])
{
	I2C_Init(i2c);
	USART_Init(usart);
	DMA_Init(dma);


	xTaskCreate(GPS_task(state, dma, dma_usartBuffer), "GPS", 1000, NULL, 1, NULL);

	vTaskStartScheduler();

	return 0;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
