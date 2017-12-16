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


inline static char _read_dma_buffer(void)
{
	// ждем, пока ДМА чего-нибудь скачает
	while(_dma_carret == (size_t)(GPS_DMA_BUFFER_SIZE - GPS_DMA_USART_STREAM->NDTR)) {}

	char retval = _dma_buffer[_dma_carret];

	_dma_carret++;
	if (_dma_carret >= GPS_DMA_BUFFER_SIZE)
		_dma_carret = 0;

	return retval;
}


void GPS_task(state_t* state, DMA_HandleTypeDef* dma, uint8_t* termBuffer)	{

	_dma_carret = 0;
	_msg_carret = 0;

	//	TODO: SET LENGTH OF DMA REQUEST
	// DMA start
	HAL_DMA_Start(dma, USART2->DR, termBuffer, 1);		//	from USART2->DR (data register) to our circular buffer

	//	TODO: SET DEVICE CONFIG (UBLOX CENTER)

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

		float _lon = minmea_tofloat(&frame.longitude);
		float _lat = minmea_tofloat(&frame.latitude);
		float _height = minmea_tofloat(&frame.altitude);
		bool _hasFix = frame.fix_quality != 0;

		taskENTER_CRITICAL();

		state->isc.coord_GPS[0] = _lon;
		state->isc.coord_GPS[1] = _lat;
		state->isc.coord_GPS[2] = _height;

		taskEXIT_CRITICAL();

	}
}

