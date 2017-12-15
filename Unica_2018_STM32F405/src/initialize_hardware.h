/*
 * initialize_hardware.h
 *
 *  Created on: 22 нояб. 2017 г.
 *      Author: developer
 */

#ifndef INITIALIZE_HARDWARE_H_
#define INITIALIZE_HARDWARE_H_


/*
 * I2C bus initialisation
 */
void I2C_Init(I2C_HandleTypeDef* i2c);
void HAL_USART_MspInit(USART_HandleTypeDef *husart);


#endif /* INITIALIZE_HARDWARE_H_ */
