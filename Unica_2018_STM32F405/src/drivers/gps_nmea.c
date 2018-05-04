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
static char _dma_buffer[GPS_DMA_BUFFER_SIZE];

static size_t _msg_carret;
static char _msg_buffer[GPS_MSG_BUFFER_SIZE];

//uint8_t dma_usartBuffer[100];

inline static char _read_dma_buffer(void)
{
	// ждем, пока ДМА чего-нибудь скачает
//	while(_dma_carret == (size_t)(GPS_DMA_BUFFER_SIZE - GPS_DMA_USART_STREAM->NDTR)) {}
	while((DMA1->HISR & (1 << 11)) != 1) {}
	printf("OK");

	char retval = _dma_buffer[_dma_carret];

	_dma_carret++;
	if (_dma_carret >= GPS_DMA_BUFFER_SIZE)
		_dma_carret = 0;

	return retval;
}


void GPS_task()	{

//	(void)args;
	//	Инициализация USART2 для работы с GPS
	usart_GPS.Init.BaudRate = 9600;
	usart_GPS.Init.WordLength = UART_WORDLENGTH_8B;
	usart_GPS.Init.StopBits = UART_STOPBITS_1;
	usart_GPS.Init.Parity = UART_PARITY_NONE;
	usart_GPS.Init.Mode = UART_MODE_TX_RX;

	usart_GPS.Instance = USART2;

	HAL_USART_Init(&usart_GPS);

	// Конфигурация NEO-7m
//	uint8_t portId = 1;				//	UART 1
//	uint8_t inProto = (1 << 1);		// NMEA
//	uint8_t outProto = (1 << 1);	// NMEA
//	uint16_t baudrate = 9600;
//	uint8_t autobauding = 0;
	char* msg = "$PUBX,41,1,0001,0001,9600,0*14\r\n";
	HAL_USART_Transmit(&usart_GPS, (uint8_t*)msg, strlen(msg), 2000);
	const TickType_t _delay = 1000 / portTICK_RATE_MS;
	vTaskDelay(_delay);

	__HAL_RCC_DMA1_CLK_ENABLE();

	//	Инициализация DMA1_Stream5 для работы c GPS через USART
	dma_GPS.Init.Channel = DMA_CHANNEL_4;						// 4 канал - на USART2_RX
	dma_GPS.Init.Direction = DMA_PERIPH_TO_MEMORY;				// направление - из периферии в память
	dma_GPS.Init.PeriphInc = DMA_PINC_DISABLE;					// инкрементация периферии выключена
	dma_GPS.Init.MemInc = DMA_MINC_ENABLE;						// инкрементация памяти включена
	dma_GPS.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;		// длина слова в периферии - байт
	dma_GPS.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;		// длина слова в памяти - байт
	dma_GPS.Init.Mode = DMA_CIRCULAR;							// режим - обычный
	dma_GPS.Init.Priority = DMA_PRIORITY_LOW;				// приоритет - средний
	dma_GPS.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//	dma_GPS.Init.FIFOThreshold = ;
	dma_GPS.Init.MemBurst = DMA_MBURST_SINGLE;
	dma_GPS.Init.PeriphBurst = DMA_PBURST_SINGLE;

	dma_GPS.Instance = DMA1_Stream5;

	HAL_DMA_Init(&dma_GPS);

	// DMA start
	memset(_dma_buffer, 0x00, GPS_DMA_BUFFER_SIZE);
	uint8_t dmaStart_error = HAL_DMA_Start(&dma_GPS, /*(uint32_t)&*/(USART2->DR), (uint32_t)&_dma_buffer[0], 1);	//	from USART2->DR (data register) to our circular buffer
	printf("start_error = %d\r\n", dmaStart_error);

	_dma_carret = 0;
	_msg_carret = 0;

	for ( ; ; )
	{
		// ждем доллара
		do {
			_msg_buffer[0] = _read_dma_buffer();
		} while (_msg_buffer[0] != '$');

		printf("sdfkljdsfslkdfjlsdkfjsldkf\n");

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

		float _lon = minmea_tofloat(&frame.longitude);
		float _lat = minmea_tofloat(&frame.latitude);
		float _height = minmea_tofloat(&frame.altitude);

		taskENTER_CRITICAL();

		stateGPS.coordinates[0] = _lon;
		stateGPS.coordinates[1] = _lat;
		stateGPS.coordinates[2] = _height;

		printf("x: &%f, y: %f, z: %f\n", stateGPS.coordinates[0], stateGPS.coordinates[1], stateGPS.coordinates[2]);

		taskEXIT_CRITICAL();

	}
}
