/*
 * nRF24L01.c
 *
 *  Created on: 10 марта 2018 г.
 *      Author: developer
 */

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#include <stdio.h>

#include "spi.h"

#include "nRF24L01.h"


static void _cs_enable(){
	nRF24L01_CS_PORT &= ~nRF24L01_CS_PIN;
}


static void _cs_disable(){
	nRF24L01_CS_PORT |= nRF24L01_CS_PIN;
}

static void _ce_enable(){
	nRF24L01_CE_PORT |= nRF24L01_CE_PIN;
}


static void _ce_disable(){
	nRF24L01_CE_PORT &= ~nRF24L01_CE_PIN;
}

void nRF24L01_send_command(uint8_t command, uint8_t* buffer, uint8_t buffer_length) {
	_cs_enable();
	rscs_spi_write(&command, 1);
	rscs_spi_read(buffer, buffer_length, 0xFF);
	_cs_disable();
}


void nRF24L01_init (){

	//Иницилизация SPI
	rscs_spi_init();
	rscs_spi_set_pol(RSCS_SPI_POL_SAMPLE_RISE_SETUP_FALL);
	rscs_spi_set_order(RSCS_SPI_ORDER_MSB_FIRST);
	rscs_spi_set_clk(400);

	// Настраиваем CS
	_cs_disable();
	_ce_enable();
	DDRB |= nRF24L01_CS_PIN;
	DDRB |= nRF24L01_CE_PIN;

	uint8_t value;

	//Запись типа (0 << PRIM_RX) не работает, но написана для ясности наших намерений
	value = (0 << MASK_RX_DR) |
			(0 << MASK_TX_DS) |
			(0 << MASK_MAX_RT) |
			(1 << EN_CRC) |
			(1 << CRCO) |
			(1 << PWR_UP) |
			(0 << PRIM_RX);
	nRF24L01_write_register(nRF24L01_CONFIG_ADDR, value);
//	printf("CONFIG = 0x%X\n", nRF24L01_read_register(nRF24L01_CONFIG_ADDR));

	value = (1 << ENAA_P5)|
			(1 << ENAA_P4)|
			(1 << ENAA_P3)|
			(1 << ENAA_P2)|
			(1 << ENAA_P1)|
			(1 << ENAA_P0);
	nRF24L01_write_register(nRF24L01_EN_AA_ADDR, value);
//	printf("EN_AA = 0x%X\n", nRF24L01_read_register(nRF24L01_EN_AA_ADDR));

	value = (1 << ERX_P5)|
			(1 << ERX_P4)|
			(1 << ERX_P3)|
			(1 << ERX_P2)|
			(1 << ERX_P1)|
			(1 << ERX_P0);
	nRF24L01_write_register(nRF24L01_EN_RXADDR_ADDR, value);
//	printf("EN_RXADDR = 0x%X\n", nRF24L01_read_register(nRF24L01_EN_RXADDR_ADDR));

	value = (0b11 << AW);
	nRF24L01_write_register(nRF24L01_SETUP_AW_ADDR, value);
//	printf("SETUP_AW = 0x%X\n", nRF24L01_read_register(nRF24L01_SETUP_AW_ADDR));

	value = (0b0011 << ARD)|
			(0b0000 << ARC);
	nRF24L01_write_register(nRF24L01_ARC_CNT_ADDR, value);
//	printf("ARC_CNT = 0x%X\n", nRF24L01_read_register(nRF24L01_ARC_CNT_ADDR));



	value = (0x4c << RF_CH);
	nRF24L01_write_register(nRF24L01_RF_CH_ADDR, value);
//	printf("RF_CH = 0x%X\n", nRF24L01_read_register(nRF24L01_RF_CH_ADDR));

	value = (0 << RF_DR)|
			(0b11 << RF_PWR)|
			(0 << LNA_HCURR);
	nRF24L01_write_register(nRF24L01_RF_SETUP_ADDR, value);
//	printf("RF_SETUP = 0x%X\n", nRF24L01_read_register(nRF24L01_RF_SETUP_ADDR));

	value = (1 << DPL_P5)|
			(1 << DPL_P4)|
			(1 << DPL_P3)|
			(1 << DPL_P2)|
			(1 << DPL_P1)|
			(1 << DPL_P0);
	nRF24L01_write_register(nRF24L01_DYNPD_ADDR, value);
//	printf("DYNPD = 0x%X\n", nRF24L01_read_register(nRF24L01_DYNPD_ADDR));

	value = (1 << EN_DPL)|
			(1 << EN_ACK_PAY)|
			(1 << EN_DYN_ACK);
	nRF24L01_write_register(nRF24L01_FEATURE_ADDR, value);
//	printf("FEATURE = 0x%X\n", nRF24L01_read_register(nRF24L01_FEATURE_ADDR));

	uint8_t device_address_P0[5] = nRF24L01_RX_ADDR_P0;
	nRF24L01_write_register_address(nRF24L01_RX_ADDR_P0_ADDR, device_address_P0, 5);
	nRF24L01_read_register_address(nRF24L01_RX_ADDR_P0_ADDR, device_address_P0, 5);
//	printf("RX_ADDR_P0 = 0x%X%X%X%X%X\n", device_address_P0[0], device_address_P0[1], device_address_P0[2], device_address_P0[3], device_address_P0[4]);

	uint8_t device_address_TX[5] = nRF24L01_TX_ADDR;
	nRF24L01_write_register_address(nRF24L01_TX_ADDR_ADDR, device_address_TX, 5);
	nRF24L01_read_register_address(nRF24L01_TX_ADDR_ADDR, device_address_TX, 5);
//	printf("TX_ADDR = 0x%X%X%X%X%X\n", device_address_P0[0], device_address_P0[1], device_address_P0[2], device_address_P0[3], device_address_P0[4]);

	value = nRF24L01_RX_PW_P0;
	nRF24L01_write_register(nRF24L01_RX_PW_P0_ADDR, value);
//	printf("RX_PW_P0 = 0x%X\n", nRF24L01_read_register(nRF24L01_RX_PW_P0_ADDR));
}

bool nRF24L01_read (void * read_buffer, size_t buffer_size){
	nRF24L01_RX_mode_on(true);
	uint8_t status = nRF24L01_read_status();

	uint8_t read_command = nRF24L01_READ_RX_FIFO;
	if (status & (1 << RX_DR))
	{
		_cs_enable();
		rscs_spi_write(&read_command, 1);
		rscs_spi_read(read_buffer, buffer_size, 0xFF);
		_cs_disable();
		//nRF24L01_clear_status(true, false, false);
		nRF24L01_RX_mode_on(false);
		return true;
	}
	nRF24L01_RX_mode_on(false);
	return false;
}

void nRF24L01_write (void * write_buffer, size_t buffer_size, bool ACK){
	_cs_enable();
	uint8_t write_command;
	if (ACK) write_command = nRF24L01_WRITE_TX_FIFO;
	else write_command = nRF24L01_WRITE_TX_FIFO_NO_ACK;
	rscs_spi_write(&write_command, 1);
	rscs_spi_write(write_buffer, buffer_size);
	_ce_enable();
	for (volatile int i = 0; i < 1000; i++) {}
	_ce_disable();
	_cs_disable();
}

void nRF24L01_write_register_address (nRF24L01_registr_addr_t address, uint8_t * data_register, size_t data_len){
	_cs_enable();
	uint8_t n = 1 + data_len;
	uint8_t buffer[n];
	buffer[0] = nRF24L01_WRITE_REGISTER(address);
	for (int i = 0; i < data_len; i++) buffer[i + 1] = data_register[i];
	rscs_spi_write(buffer, n);
	_cs_disable();
}

void nRF24L01_write_register (nRF24L01_registr_addr_t address, uint8_t data_register){
	_cs_enable();
	uint8_t buffer[2];
	buffer[0] = nRF24L01_WRITE_REGISTER(address);
	buffer[1] = data_register;
	rscs_spi_write(buffer, 2);
	_cs_disable();
}

uint8_t nRF24L01_read_register (nRF24L01_registr_addr_t address){
	_cs_enable();
	uint8_t command = nRF24L01_READ_REGISTER(address);
	uint8_t data_register;
	rscs_spi_write(&command, 1);
	rscs_spi_read(&data_register, 1, 0xFF);
	_cs_disable();
	return data_register;
}

void nRF24L01_read_register_address (nRF24L01_registr_addr_t address, uint8_t * data_register, size_t data_len){
	_cs_enable();
	uint8_t command = nRF24L01_READ_REGISTER(address);
	rscs_spi_write(&command, 1);
	rscs_spi_read(data_register, data_len, 0xFF);
	_cs_disable();
}

uint8_t nRF24L01_read_status()
{
	_cs_enable();
	uint8_t status = rscs_spi_do(0xFF);
	_cs_disable();
	return status;
}


void nRF24L01_RX_mode_on (bool mode) {
	_cs_enable();
	uint8_t config = nRF24L01_read_register(nRF24L01_CONFIG_ADDR);
	if (mode) config |= (1 << PRIM_RX);
	else config &= ~(1 << PRIM_RX);
	nRF24L01_write_register(nRF24L01_CONFIG_ADDR, config);
	_cs_disable();
}

void nRF24L01_clear_status (bool flag_RX_DR, bool flag_TX_DS,bool flag_MAX_RT) {
	_ce_disable();	//Не факт, что это нужно

	_cs_enable();
	uint8_t NOP = 0xFF;
	rscs_spi_write(&NOP, 1);
	uint8_t status = 0;
	_cs_disable();

	_cs_enable();
	if (flag_RX_DR) status |= (1 << RX_DR);
	if (flag_TX_DS) status |= (1 << TX_DS);
	if (flag_MAX_RT) status |= (1 << MAX_RT);
	nRF24L01_write_register(nRF24L01_STATUS_ADDR, status);
	_cs_disable();

	_ce_enable();	//Не факт, что это нужно
}

void nRF24L01_clear_TX_FIFO () {
	_cs_enable();
	uint8_t command = nRF24L01_FLUSH_TX_FIFO;
	rscs_spi_write(&command, 1);
	_cs_disable();
}

void nRF24L01_clear_RX_FIFO () {
	_cs_enable();
	uint8_t command = nRF24L01_FLUSH_RX_FIFO;
	rscs_spi_write(&command, 1);
	_cs_disable();
}



