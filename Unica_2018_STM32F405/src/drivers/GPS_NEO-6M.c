/*
 * GPS_NEO-6M.c
 *
 *  Created on: 29 нояб. 2017 г.
 *      Author: developer
 */

#include "state.h"
#include "minmea.h"
#include "GPS_NEO-6M.h"

//	тип сообщения - RMC
//TODO: настроить датчик (ublox center)

//int neo6m_read(wires_handle_t* handle, struct minmea_sentence_rmc* rmcData) {
//
//	int error = 0;
//	uint8_t receivedData;
//
//	PROCESS_ERROR(HAL_USART_Receive(handle->usart, &receivedData, 82, 2000));
//	if (!minmea_parse_rmc(rmcData, &receivedData)) return -1;
//
//
//end:
//	return error;
//}
