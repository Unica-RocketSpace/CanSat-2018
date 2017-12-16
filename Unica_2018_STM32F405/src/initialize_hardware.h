/*
 * initialize_hardware.h
 *
 *  Created on: 22 нояб. 2017 г.
 *      Author: developer
 */

#ifndef INITIALIZE_HARDWARE_H_
#define INITIALIZE_HARDWARE_H_

#include <stm32f4xx_hal_i2c.h>
#include <stm32f4xx_hal_usart.h>
#include <stm32f4xx_hal_dma.h>

void I2C_Init(I2C_HandleTypeDef* i2c);
void USART_Init(USART_HandleTypeDef * usart);
void DMA_Init(DMA_HandleTypeDef* dma);


#endif /* INITIALIZE_HARDWARE_H_ */
