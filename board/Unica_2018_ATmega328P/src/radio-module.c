
#include <stdbool.h>
#include <stdio.h>

#include "nRF24L01.h"
#include "spi.h"

#include "radio-module.h"
#include "state.h"

void send_package() {
	//Если TX буфер переполнен, то отчищаем его
	uint8_t status = nRF24L01_read_status();
	if (status & (1 << TX_FULL)) nRF24L01_clear_TX_FIFO();

	//Перед отправкой сообщения отчищаем STATUS от старых флагов
	nRF24L01_clear_status(false, true, true);

	//Передаем сообщение
	nRF24L01_write(&TM_package, sizeof(TM_package), true);
	TM_package.package_number++;
};

uint8_t get_package(void * RX_buffer) {
	//читаем длину пакета в RX буфере nRF24L01
	uint8_t RX_buffer_len;
	uint8_t command = nRF24L01_R_RX_PL_WID;
	nRF24L01_send_command(command, &RX_buffer_len, 1);

	//читаем данные из RX буфера
	bool is_data = nRF24L01_read(RX_buffer, RX_buffer_len);

	//Почему-то после чтения из RX буфера радиомодуль переходит в режим Power Down
	//Поэтому каждый раз пробуждаем его
	uint8_t value = nRF24L01_read_register(nRF24L01_CONFIG_ADDR);
	value |= (1 << PWR_UP);
	nRF24L01_write_register(nRF24L01_CONFIG_ADDR, value);

	//если данных нет, то возвращаем 0
	if (!is_data) return 0;

	//если данные есть, то возвращаем длину пакета
	return RX_buffer_len;
};


