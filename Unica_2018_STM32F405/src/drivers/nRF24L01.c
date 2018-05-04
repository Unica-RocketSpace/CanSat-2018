/*
 * nRF24L01.c
 *
 *  Created on: 10 марта 2018 г.
 *      Author: developer
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include <stm32f4xx_hal.h>

#include "FreeRTOS.h"
#include "task.h"

#include "nRF24L01.h"
#include "state.h"

#define _TIMEOUT_	1000

static void _cs_enable(){
//	nRF24L01_CS_PORT &= ~nRF24L01_CS_PIN;
	HAL_GPIO_WritePin(nRF24L01_CS_PORT, nRF24L01_CS_PIN, RESET);
}

static void _cs_disable(){
//	nRF24L01_CS_PORT |= nRF24L01_CS_PIN;
	HAL_GPIO_WritePin(nRF24L01_CS_PORT, nRF24L01_CS_PIN, SET);
}

static void _ce_enable(){
//	nRF24L01_CE_PORT |= nRF24L01_CE_PIN;
	HAL_GPIO_WritePin(nRF24L01_CE_PORT, nRF24L01_CE_PIN, SET);
}

static void _ce_disable(){
//	nRF24L01_CE_PORT &= ~nRF24L01_CE_PIN;
	HAL_GPIO_WritePin(nRF24L01_CE_PORT, nRF24L01_CE_PIN, RESET);
}


uint8_t nRF24L01_init (SPI_HandleTypeDef* hspi){
	uint8_t error = 0;

	//Иницилизация SPI
	hspi->Instance = SPI1;
	hspi->Init.Mode = SPI_MODE_MASTER;
	hspi->Init.Direction = SPI_DIRECTION_2LINES;
	hspi->Init.DataSize = SPI_DATASIZE_8BIT;
	hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi->Init.NSS = SPI_NSS_SOFT;
	hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi->Init.TIMode = SPI_TIMODE_DISABLE;
	hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;

	PROCESS_ERROR(HAL_SPI_Init(hspi));
	vTaskDelay(100/portTICK_RATE_MS);

	// Настраиваем CS
	_cs_disable();
	_ce_enable();

	uint8_t value;

	//Запись типа (0 << PRIM_RX) не работает, но написана для ясности наших намерений
	value = (0 << MASK_RX_DR) |
			(0 << MASK_TX_DS) |
			(0 << MASK_MAX_RT) |
			(1 << EN_CRC) |
			(1 << CRCO) |
			(1 << PWR_UP) |
			(0 << PRIM_RX);
	PROCESS_ERROR(nRF24L01_write_register(hspi, nRF24L01_CONFIG_ADDR, value));
	value = 0;
	nRF24L01_read_register(hspi, nRF24L01_CONFIG_ADDR, &value);
	printf("config = %d", value);

	value = (1 << ENAA_P5)|
			(1 << ENAA_P4)|
			(1 << ENAA_P3)|
			(1 << ENAA_P2)|
			(1 << ENAA_P1)|
			(1 << ENAA_P0);
	PROCESS_ERROR(nRF24L01_write_register(hspi, nRF24L01_EN_AA_ADDR, value));
	value = 0;
	nRF24L01_read_register(hspi, nRF24L01_EN_AA_ADDR, &value);
	printf("en_aa = %d", value);

	value = (1 << ERX_P5)|
			(1 << ERX_P4)|
			(1 << ERX_P3)|
			(1 << ERX_P2)|
			(1 << ERX_P1)|
			(1 << ERX_P0);
	PROCESS_ERROR(nRF24L01_write_register(hspi, nRF24L01_EN_RXADDR_ADDR, value));
	value = 0;
	nRF24L01_read_register(hspi, nRF24L01_EN_RXADDR_ADDR, &value);
	printf("en_rxaddr = %d", value);

	value = (0b11 << AW);
	PROCESS_ERROR(nRF24L01_write_register(hspi, nRF24L01_SETUP_AW_ADDR, value));
	value = 0;
	nRF24L01_read_register(hspi, nRF24L01_SETUP_AW_ADDR, &value);
	printf("setup_aw = %d", value);

	value = (0b0011 << ARD)|
			(0b1111 << ARC);
	PROCESS_ERROR(nRF24L01_write_register(hspi, nRF24L01_ARC_CNT_ADDR, value));
	value = 0;
	nRF24L01_read_register(hspi, nRF24L01_ARC_CNT_ADDR, &value);
	printf("en_rxaddr = %d", value);


	value = (0x4c << RF_CH);
	PROCESS_ERROR(nRF24L01_write_register(hspi, nRF24L01_RF_CH_ADDR, value));
	value = 0;
	nRF24L01_read_register(hspi, nRF24L01_RF_CH_ADDR, &value);
	printf("rf_ch = %d", value);

	value = (0 << RF_DR)|
			(0b11 << RF_PWR)|
			(0 << LNA_HCURR);
	PROCESS_ERROR(nRF24L01_write_register(hspi, nRF24L01_RF_SETUP_ADDR, value));
	value = 0;
	nRF24L01_read_register(hspi, nRF24L01_RF_SETUP_ADDR, &value);
	printf("rf_setup = %d", value);

	value = (1 << DPL_P5)|
			(1 << DPL_P4)|
			(1 << DPL_P3)|
			(1 << DPL_P2)|
			(1 << DPL_P1)|
			(1 << DPL_P0);
	PROCESS_ERROR(nRF24L01_write_register(hspi, nRF24L01_DYNPD_ADDR, value));
	value = 0;
	nRF24L01_read_register(hspi, nRF24L01_DYNPD_ADDR, &value);
	printf("dynpd = %d", value);

	value = (1 << EN_DPL)|
			(1 << EN_ACK_PAY)|
			(1 << EN_DYN_ACK);
	PROCESS_ERROR(nRF24L01_write_register(hspi, nRF24L01_FEATURE_ADDR, value));
	value = 0;
	nRF24L01_read_register(hspi, nRF24L01_FEATURE_ADDR, &value);
	printf("feature = %d", value);

	uint8_t device_address_P0[5] = nRF24L01_RX_ADDR_P0;
	PROCESS_ERROR(nRF24L01_write_register_address(hspi, nRF24L01_RX_ADDR_P0_ADDR, device_address_P0, 5));
	value = 0;
	nRF24L01_read_register(hspi, nRF24L01_RX_ADDR_P0_ADDR, &value);
	printf("rx = %d", value);

	uint8_t device_address_TX[5] = nRF24L01_TX_ADDR;
	PROCESS_ERROR(nRF24L01_write_register_address(hspi, nRF24L01_TX_ADDR_ADDR, device_address_TX, 5));
	value = 0;
	nRF24L01_read_register(hspi, nRF24L01_TX_ADDR_ADDR, &value);
	printf("tx = %d", value);

	value = nRF24L01_RX_PW_P0;
	PROCESS_ERROR(nRF24L01_write_register(hspi, nRF24L01_RX_PW_P0_ADDR, value));
	value = 0;
	nRF24L01_read_register(hspi, nRF24L01_RX_PW_P0_ADDR, &value);
	printf("rx_pw = %d", value);

end:
	return error;
}

//FIXME: void* or uint8_t*
uint8_t nRF24L01_read (SPI_HandleTypeDef* hspi, uint8_t * read_buffer, size_t buffer_size, bool* isData){
	uint8_t error = 0;
	PROCESS_ERROR(nRF24L01_RX_mode_on(hspi, true));
	uint8_t status = 0;
	PROCESS_ERROR(nRF24L01_read_status(hspi, &status));

	uint8_t read_command = nRF24L01_READ_RX_FIFO;
	if (status & (1 << RX_DR))
	{
		_cs_enable();
		PROCESS_ERROR(HAL_SPI_Transmit(hspi, &read_command, 1, _TIMEOUT_));
		PROCESS_ERROR(HAL_SPI_Receive(hspi, read_buffer, buffer_size, _TIMEOUT_));
		_cs_disable();
		PROCESS_ERROR(nRF24L01_clear_status(hspi, true, false, false));
		PROCESS_ERROR(nRF24L01_RX_mode_on(hspi, false));
		*isData = 1;
	}
	PROCESS_ERROR(nRF24L01_RX_mode_on(hspi, false));
	*isData = 0;
end:
	return error;
}

uint8_t nRF24L01_write (SPI_HandleTypeDef* hspi, void * write_buffer, size_t buffer_size, bool ACK){
	uint8_t error = 0;
	_cs_enable();
	uint8_t write_command;
	if (ACK) write_command = nRF24L01_WRITE_TX_FIFO;
	else write_command = nRF24L01_WRITE_TX_FIFO_NO_ACK;
	PROCESS_ERROR(HAL_SPI_Transmit(hspi, &write_command, 1, _TIMEOUT_));
	PROCESS_ERROR(HAL_SPI_Transmit(hspi, write_buffer, buffer_size, _TIMEOUT_));
	_cs_disable();
end:
	return error;
}

uint8_t nRF24L01_write_register_address (SPI_HandleTypeDef* hspi, nRF24L01_registr_addr_t address, uint8_t * data_register, size_t data_len){
	uint8_t error = 0;
	_cs_enable();
	uint8_t n = 1 + data_len;
	uint8_t buffer[n];
	buffer[0] = nRF24L01_WRITE_REGISTER(address);
	for (int i = 0; i < (int)data_len; i++) {
		buffer[i + 1] = data_register[i];
	}
	PROCESS_ERROR(HAL_SPI_Transmit(hspi, buffer, n, _TIMEOUT_));
	_cs_disable();
end:
	return error;
}

uint8_t nRF24L01_write_register (SPI_HandleTypeDef* hspi, nRF24L01_registr_addr_t address, uint8_t data_register){
	uint8_t error = 0;
	_cs_enable();
	uint8_t buffer[2];
	buffer[0] = nRF24L01_WRITE_REGISTER(address);
	buffer[1] = data_register;
	PROCESS_ERROR(HAL_SPI_Transmit(hspi, buffer, sizeof(buffer), _TIMEOUT_));
	_cs_disable();
end:
	return error;
}

uint8_t nRF24L01_read_register (SPI_HandleTypeDef* hspi, nRF24L01_registr_addr_t address, uint8_t* data){
	uint8_t error = 0;
	_cs_enable();
	uint8_t command = nRF24L01_READ_REGISTER(address);
	PROCESS_ERROR(HAL_SPI_Transmit(hspi, &command, 1, _TIMEOUT_));
	PROCESS_ERROR(HAL_SPI_Receive(hspi, data, sizeof(*data), _TIMEOUT_));
	_cs_disable();
end:
	return error;
}

uint8_t nRF24L01_read_register_address (SPI_HandleTypeDef* hspi, nRF24L01_registr_addr_t address, uint8_t * data_register, size_t data_len){
	uint8_t error = 0;
	_cs_enable();
	uint8_t command = nRF24L01_READ_REGISTER(address);
	PROCESS_ERROR(HAL_SPI_Transmit(hspi, &command, 1, _TIMEOUT_));
	PROCESS_ERROR(HAL_SPI_Receive(hspi, data_register, data_len, _TIMEOUT_));
	_cs_disable();
end:
	return error;
}

uint8_t nRF24L01_read_status(SPI_HandleTypeDef* hspi, uint8_t* status){
	uint8_t error = 0;
	_cs_enable();
//	status = 0x00;
	uint8_t dummy = 0xFF;
	PROCESS_ERROR(HAL_SPI_TransmitReceive(hspi, &dummy, status, 1, _TIMEOUT_));
	_cs_disable();
end:
	return error;
}

uint8_t nRF24L01_RX_mode_on (SPI_HandleTypeDef* hspi, bool mode) {
	uint8_t error = 0;
	_cs_enable();
	uint8_t config = 0;
	PROCESS_ERROR(nRF24L01_read_register(hspi, nRF24L01_CONFIG_ADDR, config));
	if (mode) config |= (1 << PRIM_RX);
	else config &= ~(1 << PRIM_RX);
	PROCESS_ERROR(nRF24L01_write_register(hspi, nRF24L01_CONFIG_ADDR, config));
	_cs_disable();
end:
	return error;
}

uint8_t nRF24L01_clear_status (SPI_HandleTypeDef* hspi, bool flag_RX_DR, bool flag_TX_DS,bool flag_MAX_RT) {
	uint8_t error = 0;
	_ce_disable();	//Не факт, что это нужно

	_cs_enable();
	uint8_t NOP = 0xFF;
	PROCESS_ERROR(HAL_SPI_Transmit(hspi, &NOP, 1, _TIMEOUT_));
	uint8_t status = 0;
	_cs_disable();

	_cs_enable();
	if (flag_RX_DR) status |= (1 << RX_DR);
	if (flag_TX_DS) status |= (1 << TX_DS);
	if (flag_MAX_RT) status |= (1 << MAX_RT);
	PROCESS_ERROR(nRF24L01_write_register(hspi, nRF24L01_STATUS_ADDR, status));
	_cs_disable();

	_ce_enable();	//Не факт, что это нужно
end:
	return error;
}

uint8_t nRF24L01_clear_TX_FIFO (SPI_HandleTypeDef* hspi) {
	uint8_t error = 0;
	_cs_enable();
	uint8_t command = nRF24L01_FLUSH_TX_FIFO;
	PROCESS_ERROR(HAL_SPI_Transmit(hspi, &command, 1, _TIMEOUT_));
	_cs_disable();
end:
	return error;
}

uint8_t nRF24L01_clear_RX_FIFO (SPI_HandleTypeDef* hspi) {
	uint8_t error = 0;
	_cs_enable();
	uint8_t command = nRF24L01_FLUSH_RX_FIFO;
	PROCESS_ERROR(HAL_SPI_Transmit(hspi, &command, 1, _TIMEOUT_));
	_cs_disable();
end:
	return error;
}



