
#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "Timer.h"
#include "BlinkLed.h"

//#include "state.h"
#include "initialize_hardware.h"
//#include "stm32f4xx_hal.h"



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


state_t* state;
state_t* state_prev;

I2C_HandleTypeDef* i2c;
USART_HandleTypeDef* usart;

void USART2_IRQHandler(void) {

	if (USART2->SR & USART_SR_RXNE & USART_SR_ORE) {



	}


}


int main(int argc, char* argv[])
{
	I2C_Init(i2c);
	USART_Init(usart);


	// разрешение прерывания RXNE (read data register is not empty)
	USART2->CR1 |= (1 << USART_CR1_RXNEIE);

	HAL_NVIC_EnableIRQ(USART2_IRQn);


}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
