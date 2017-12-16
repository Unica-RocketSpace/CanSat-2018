/*
 * gps_nmea.h
 *
 *  Created on: 16 дек. 2017 г.
 *      Author: developer
 */

#ifndef GPS_NMEA_H_
#define GPS_NMEA_H_

#include <stm32f4xx_hal.h>

#define GPS_DMA_BUFFER_SIZE		(100)

#define GPS_USART 				(USART2)
#define GPS_DMA_BUFFER_SIZE 	(500)
#define GPS_MSG_BUFFER_SIZE 	(200)
#define GPS_DMA_USART_STREAM 	(DMA1_Stream5)


void GPS_task(state_t* state, DMA_HandleTypeDef* dma, uint8_t* termBuffer);


#endif /* GPS_NMEA_H_ */
