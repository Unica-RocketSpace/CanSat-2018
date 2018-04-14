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

#include "nRF24L01.h"

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


void nRF24L01_init (SPI_HandleTypeDef* hspi){

	//Иницилизация SPI
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

	hspi->Instance = SPI1;
	HAL_SPI_Init(hspi);

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
	nRF24L01_write_register(hspi, nRF24L01_CONFIG_ADDR, value);

	value = (1 << ENAA_P5)|
			(1 << ENAA_P4)|
			(1 << ENAA_P3)|
			(1 << ENAA_P2)|
			(1 << ENAA_P1)|
			(1 << ENAA_P0);
	nRF24L01_write_register(hspi, nRF24L01_EN_AA_ADDR, value);

	value = (1 << ERX_P5)|
			(1 << ERX_P4)|
			(1 << ERX_P3)|
			(1 << ERX_P2)|
			(1 << ERX_P1)|
			(1 << ERX_P0);
	nRF24L01_write_register(hspi, nRF24L01_EN_RXADDR_ADDR, value);

	value = (0b11 << AW);
	nRF24L01_write_register(hspi, nRF24L01_SETUP_AW_ADDR, value);

	value = (0b0011 << ARD)|
			(0b1111 << ARC);
	nRF24L01_write_register(hspi, nRF24L01_ARC_CNT_ADDR, value);



	value = (0x4c << RF_CH);
	nRF24L01_write_register(hspi, nRF24L01_RF_CH_ADDR, value);

	value = (0 << RF_DR)|
			(0b11 << RF_PWR)|
			(0 << LNA_HCURR);
	nRF24L01_write_register(hspi, nRF24L01_RF_SETUP_ADDR, value);

	value = (1 << DPL_P5)|
			(1 << DPL_P4)|
			(1 << DPL_P3)|
			(1 << DPL_P2)|
			(1 << DPL_P1)|
			(1 << DPL_P0);
	nRF24L01_write_register(hspi, nRF24L01_DYNPD_ADDR, value);

	value = (1 << EN_DPL)|
			(1 << EN_ACK_PAY)|
			(1 << EN_DYN_ACK);
	nRF24L01_write_register(hspi, nRF24L01_FEATURE_ADDR, value);

	uint8_t device_address_P0[5] = nRF24L01_RX_ADDR_P0;
	nRF24L01_write_register_address(hspi, nRF24L01_RX_ADDR_P0_ADDR, device_address_P0, 5);

	uint8_t device_address_TX[5] = nRF24L01_TX_ADDR;
	nRF24L01_write_register_address(hspi, nRF24L01_TX_ADDR_ADDR, device_address_TX, 5);

	value = nRF24L01_RX_PW_P0;
	nRF24L01_write_register(hspi, nRF24L01_RX_PW_P0_ADDR, value);
}

//FIXME: void* or uint8_t*
bool nRF24L01_read (SPI_HandleTypeDef* hspi, uint8_t * read_buffer, size_t buffer_size){
	nRF24L01_RX_mode_on(hspi, true);
	uint8_t status = nRF24L01_read_status(hspi);

	uint8_t read_command = nRF24L01_READ_RX_FIFO;
	if (status & (1 << RX_DR))
	{
		_cs_enable();
		HAL_SPI_Transmit(hspi, &read_command, 1, _TIMEOUT_);
		HAL_SPI_Receive(hspi, read_buffer, buffer_size, _TIMEOUT_);
		_cs_disable();
		nRF24L01_clear_status(hspi, true, false, false);
		nRF24L01_RX_mode_on(hspi, false);
		return true;
	}
	nRF24L01_RX_mode_on(hspi, false);
	return false;
}

void nRF24L01_write (SPI_HandleTypeDef* hspi, void * write_buffer, size_t buffer_size, bool ACK){
	_cs_enable();
	uint8_t write_command;
	if (ACK) write_command = nRF24L01_WRITE_TX_FIFO;
	else write_command = nRF24L01_WRITE_TX_FIFO_NO_ACK;
	HAL_SPI_Transmit(hspi, &write_command, 1, _TIMEOUT_);
	HAL_SPI_Transmit(hspi, write_buffer, buffer_size, _TIMEOUT_);
	_cs_disable();
}

void nRF24L01_write_register_address (SPI_HandleTypeDef* hspi, nRF24L01_registr_addr_t address, uint8_t * data_register, size_t data_len){
	_cs_enable();
	uint8_t n = 1 + data_len;
	uint8_t buffer[n];
	buffer[0] = nRF24L01_WRITE_REGISTER(address);
	for (int i = 0; i < (int)data_len; i++) {
		buffer[i + 1] = data_register[i];
	}
	HAL_SPI_Transmit(hspi, buffer, n, _TIMEOUT_);
	_cs_disable();
}

void nRF24L01_write_register (SPI_HandleTypeDef* hspi, nRF24L01_registr_addr_t address, uint8_t data_register){
	_cs_enable();
	uint8_t buffer[2];
	buffer[0] = nRF24L01_WRITE_REGISTER(address);
	buffer[1] = data_register;
	HAL_SPI_Transmit(hspi, buffer, sizeof(buffer), _TIMEOUT_);
	_cs_disable();
}

uint8_t nRF24L01_read_register (SPI_HandleTypeDef* hspi, nRF24L01_registr_addr_t address){
	_cs_enable();
	uint8_t command = nRF24L01_READ_REGISTER(address);
	uint8_t data_register;
	HAL_SPI_Transmit(hspi, &command, sizeof(command), _TIMEOUT_);
	HAL_SPI_Receive(hspi, &data_register, sizeof(data_register), _TIMEOUT_);
	_cs_disable();
	return data_register;
}

void nRF24L01_read_register_address (SPI_HandleTypeDef* hspi, nRF24L01_registr_addr_t address, uint8_t * data_register, size_t data_len){
	_cs_enable();
	uint8_t command = nRF24L01_READ_REGISTER(address);
	HAL_SPI_Transmit(hspi, &command, sizeof(command), _TIMEOUT_);
	HAL_SPI_Receive(hspi, data_register, data_len, _TIMEOUT_);
	_cs_disable();
}

uint8_t nRF24L01_read_status(SPI_HandleTypeDef* hspi)
{
	_cs_enable();
	uint8_t status = 0x00;
	uint8_t dummy = 0xFF;
	HAL_SPI_TransmitReceive(hspi, &dummy, &status, 1, _TIMEOUT_);
	_cs_disable();
	return status;
}


void nRF24L01_RX_mode_on (SPI_HandleTypeDef* hspi, bool mode) {
	_cs_enable();
	uint8_t config = nRF24L01_read_register(hspi, nRF24L01_CONFIG_ADDR);
	if (mode) config |= (1 << PRIM_RX);
	else config &= ~(1 << PRIM_RX);
	nRF24L01_write_register(hspi, nRF24L01_CONFIG_ADDR, config);
	_cs_disable();
}

void nRF24L01_clear_status (SPI_HandleTypeDef* hspi, bool flag_RX_DR, bool flag_TX_DS,bool flag_MAX_RT) {
	_ce_disable();	//Не факт, что это нужно

	_cs_enable();
	uint8_t NOP = 0xFF;
	HAL_SPI_Transmit(hspi, &NOP, sizeof(NOP), _TIMEOUT_);
	uint8_t status = 0;
	_cs_disable();

	_cs_enable();
	if (flag_RX_DR) status |= (1 << RX_DR);
	if (flag_TX_DS) status |= (1 << TX_DS);
	if (flag_MAX_RT) status |= (1 << MAX_RT);
	nRF24L01_write_register(hspi, nRF24L01_STATUS_ADDR, status);
	_cs_disable();

	_ce_enable();	//Не факт, что это нужно
}

void nRF24L01_clear_TX_FIFO (SPI_HandleTypeDef* hspi) {
	_cs_enable();
	uint8_t command = nRF24L01_FLUSH_TX_FIFO;
	HAL_SPI_Transmit(hspi, &command, sizeof(command), _TIMEOUT_);
	_cs_disable();
}

void nRF24L01_clear_RX_FIFO (SPI_HandleTypeDef* hspi) {
	_cs_enable();
	uint8_t command = nRF24L01_FLUSH_RX_FIFO;
	HAL_SPI_Transmit(hspi, &command, sizeof(command), _TIMEOUT_);
	_cs_disable();
}



