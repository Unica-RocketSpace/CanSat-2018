/*
 * gps_nmea.c
 *
 *  Created on: 16 дек. 2017 г.
 *      Author: developer
 */

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "diag/Trace.h"

#include "FreeRTOS.h"
#include "task.h"

#include "minmea.h"
#include "gps_nmea.h"
#include "state.h"

static size_t _dma_carret;
static char _dma_buffer[GPS_DMA_BUFFER_SIZE] = {0};

static size_t _msg_carret;
static char _msg_buffer[GPS_MSG_BUFFER_SIZE];


static UART_HandleTypeDef uart_GPS;
static DMA_HandleTypeDef dma_GPS;


inline static char _read_dma_buffer(void)
{
	// ждем, пока ДМА чего-нибудь скачает
	while(_dma_carret == (size_t)(GPS_DMA_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&dma_GPS))) {}

	char retval = _dma_buffer[_dma_carret];

	_dma_carret++;
	if (_dma_carret >= GPS_DMA_BUFFER_SIZE)
		_dma_carret = 0;

	return retval;
}

uint8_t gps_initAll() {
	uint8_t error = 0;

	//	Инициализация USART2 для работы с GPS
	uart_GPS.Instance = USART2;
	uart_GPS.Init.BaudRate = 9600;
	uart_GPS.Init.WordLength = UART_WORDLENGTH_8B;
	uart_GPS.Init.StopBits = UART_STOPBITS_1;
	uart_GPS.Init.Parity = UART_PARITY_NONE;
	uart_GPS.Init.Mode = UART_MODE_TX_RX;
	uart_GPS.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart_GPS.Init.OverSampling = UART_OVERSAMPLING_16;

	PROCESS_ERROR(HAL_UART_Init(&uart_GPS));
	/* Peripheral interrupt init*/
	HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);

	vTaskDelay(100/portTICK_RATE_MS);

	__HAL_RCC_DMA1_CLK_ENABLE();
	//	Инициализация DMA1_Stream5 для работы c GPS через USART
	dma_GPS.Instance = DMA1_Stream5;
	dma_GPS.Init.Channel = DMA_CHANNEL_4;						// 4 канал - на USART2_RX
	dma_GPS.Init.Direction = DMA_PERIPH_TO_MEMORY;				// направление - из периферии в память
	dma_GPS.Init.PeriphInc = DMA_PINC_DISABLE;					// инкрементация периферии выключена
	dma_GPS.Init.MemInc = DMA_MINC_ENABLE;						// инкрементация памяти включена
	dma_GPS.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;		// длина слова в периферии - байт
	dma_GPS.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;		// длина слова в памяти - байт
	dma_GPS.Init.Mode = DMA_NORMAL;								// режим - обычный
	dma_GPS.Init.Priority = DMA_PRIORITY_MEDIUM;				// приоритет - средний
	dma_GPS.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	dma_GPS.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	dma_GPS.Init.MemBurst = DMA_MBURST_SINGLE;
	dma_GPS.Init.PeriphBurst = DMA_PBURST_SINGLE;
	PROCESS_ERROR(HAL_DMA_Init(&dma_GPS));

	__HAL_LINKDMA(&uart_GPS, hdmarx, dma_GPS);
	/* DMA interrupt init */
	HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

end:
	return error;
}

/* This function handles DMA1 stream6 global interrupt. */
void DMA1_Stream5_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&dma_GPS);
}

void USART2_IRQHandler(void)
{
	HAL_UART_IRQHandler(&uart_GPS);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2) {
		HAL_UART_Receive_DMA(huart, (uint8_t*)_dma_buffer, sizeof(_dma_buffer));
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	if(huart->Instance == USART2) {
		uint8_t gps_initError = gps_initAll();
		state_system.GPS_state = gps_initError;
		_dma_carret = 0;
		_msg_carret = 0;
		HAL_UART_RxCpltCallback(&uart_GPS);
	}
}



void GPS_task()	{

	memset(_dma_buffer, 0x00, GPS_DMA_BUFFER_SIZE);

	uint8_t gps_initError = gps_initAll();
	state_system.GPS_state = gps_initError;

	_dma_carret = 0;
	_msg_carret = 0;

	HAL_UART_RxCpltCallback(&uart_GPS);

	for ( ; ; )
	{
		// ждем доллара
		do {
			_msg_buffer[0] = _read_dma_buffer();
		} while (_msg_buffer[0] != '$');

		// Оп! нашли
		_msg_carret = 1;

		// теперь накапливаем все до \r\n
		do {
			_msg_buffer[_msg_carret++] = _read_dma_buffer();
			if (_msg_carret >= GPS_MSG_BUFFER_SIZE)
			{
				// что-то не так
				continue;
			}

		} while('\r' != _msg_buffer[_msg_carret-2]
			||	'\n' != _msg_buffer[_msg_carret-1]
		);

		// в конце терминируем строку от греха подальше
		_msg_buffer[_msg_carret] = '\x00';

		// накопили, теперь разбираем
		if (!minmea_check(_msg_buffer, false))
			continue;

		struct minmea_sentence_gga frame;
		if (!minmea_parse_gga(&frame, _msg_buffer))
			continue; // опс, что-то пошло не так

		if (frame.fix_quality == 0)
			continue;

		float _lon = minmea_tocoord(&frame.longitude);
		float _lat = minmea_tocoord(&frame.latitude);
		float _height = minmea_tofloat(&frame.altitude);

		taskENTER_CRITICAL();
		stateGPS.coordinates[0] = _lon;
		stateGPS.coordinates[1] = _lat;
		stateGPS.coordinates[2] = _height;
		taskEXIT_CRITICAL();

//		printf("x: %f, y: %f, z: %f\n", _lon, _lat, _height);
	}
}

