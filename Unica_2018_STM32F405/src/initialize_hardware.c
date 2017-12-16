/*
 * initialize_hardware.c
 *
 *  Created on: 18 нояб. 2017 г.
 *      Author: developer
 */

#include <stdint.h>
#include <stdlib.h>

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_cortex.h>
#include <stm32f4xx_hal_i2c.h>
#include <stm32f4xx_hal_usart.h>
#include <stm32f4xx_hal_dma.h>
#include <stm32f4xx_hal_gpio.h>

#include "state.h"


void I2C_Init(I2C_HandleTypeDef* i2c) {

	i2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	i2c->Init.ClockSpeed = 100000;
	i2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c->Init.DutyCycle = I2C_DUTYCYCLE_2;
	i2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	//	TODO: SET REAL
	i2c->Init.OwnAddress1 = 0xFF;
	i2c->Init.OwnAddress2 = 0xF8;

	i2c->Instance = I2C1;
	i2c->Mode = HAL_I2C_MODE_MASTER;

	HAL_I2C_Init(i2c);

}


void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
	if(hi2c->Instance == I2C1) {
		__I2C1_CLK_ENABLE();
		__GPIOB_CLK_ENABLE();

		GPIO_InitTypeDef gpiob;
		gpiob.Alternate = GPIO_AF4_I2C1;
		gpiob.Mode = GPIO_MODE_AF_PP;
		gpiob.Pin = GPIO_PIN_8 | GPIO_PIN_9;
		gpiob.Pull = GPIO_NOPULL;
		gpiob.Speed = GPIO_SPEED_FREQ_HIGH;

		HAL_GPIO_Init(GPIOB, &gpiob);
	}
	else abort();
}

void USART_Init(USART_HandleTypeDef * usart) {

	usart->Init.BaudRate = 9600;
	usart->Init.WordLength = UART_WORDLENGTH_8B;
	usart->Init.StopBits = UART_STOPBITS_2;
	usart->Init.Parity = UART_PARITY_NONE;
	usart->Init.Mode = UART_MODE_TX_RX;

	usart->Instance = USART2;

	HAL_USART_Init(usart);

}

void HAL_USART_MspInit(USART_HandleTypeDef *husart) {

	if(husart->Instance == USART2) {
		__USART2_CLK_ENABLE();
		__GPIOB_CLK_ENABLE();

		GPIO_InitTypeDef gpiob;
		gpiob.Alternate = GPIO_AF7_USART2;
		gpiob.Mode = GPIO_MODE_AF_PP;
		gpiob.Pin = GPIO_PIN_6 | GPIO_PIN_7;
		gpiob.Pull = GPIO_NOPULL;
		gpiob.Speed = GPIO_SPEED_FREQ_HIGH;

		HAL_GPIO_Init(GPIOB, &gpiob);
	}
	else abort();
}


void DMA_Init(DMA_HandleTypeDef* dma) {

	dma->Init.Channel = DMA_CHANNEL_4;							// 4 канал - на USART2_RX
	dma->Init.Direction = DMA_PERIPH_TO_MEMORY;				// направление - из периферии в память
	dma->Init.PeriphInc = DMA_PINC_DISABLE;					// инкрементация периферии выключена
	dma->Init.MemInc = DMA_MINC_ENABLE;						// инкрементация памяти включена
	dma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;		// длина слова в периферии - байт
	dma->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;			// длина слова в памяти - байт
	dma->Init.Mode = DMA_CIRCULAR;								// режим - обычный
	dma->Init.Priority = DMA_PRIORITY_MEDIUM;					// приоритет - средний
	dma->Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	dma->Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	dma->Init.MemBurst = DMA_MBURST_SINGLE;
	dma->Init.PeriphBurst = DMA_PBURST_SINGLE;

	dma->Instance = DMA1_Stream5;

	HAL_DMA_Init(dma);

	__HAL_RCC_DMA1_CLK_ENABLE();
}


