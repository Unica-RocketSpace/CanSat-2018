/*
 * main.c
 *
 *  Created on: 09 дек. 2017 г.
 *      Author: developer
 */

#include <avr/interrupt.h>
#include <avr/delay.h>

#include <stdext/stdio.h>
#include <string.h>

#include "state.h"
#include "uart.h"
#include "spi.h"
#include "i2c.h"
#include "nRF24L01.h"
#include "bmp280.h"
#include "deployment_control.h"
#include "radio-module.h"
#include "timeservice.h"

#include "mavlink-messages/mavlink/UNISAT/mavlink.h"

//***ИНИЦИАЛИЗАЦИЯ STATE***//
TM_package_t TM_package = { 0 };
initial_params_t initial_params;
raw_data_t raw_data;
const rscs_bmp280_calibration_values_t * bmp280_calibration_values;

uint8_t data_register;

char message[] = "Unica Broadcasting - ATmega328P";
uint8_t device_address[5];
uint32_t step = 0;
bool RX_get = 0;

uint8_t RX_buffer[nRF24L01_RX_BUFFER_LEN] = { 0 };

uint8_t nRF24L01_RX_BUFFER[nRF24L01_RX_BUFFER_LEN];

uint8_t RM_input_data_len;
uint8_t RM_input_data[nRF24L01_RX_BUFFER_LEN];

typedef enum {
	STAGE_INITIAL = 0,		// Состояние, в котором инициализируем все
	STAGE_LOADING = 1,		// Состояние в котором мы мы ничего не делаем и ждем команды
	STAGE_INITIAL_PARAM = 2,// Состояни, в котором мы определяем начальное давление
	STAGE_GOING_UP = 3, 	// Состояние, в котором мы летим вверх
	STAGE_GOING_DOWN = 4, 	// Состояние, в котором мы летим вниз
	STAGE_ALL_DONE = 5, 	// Мы все сделали, отдыхваем
} state_t;

int main() {

	TM_package.state = 0;

	state_t my_stage = STAGE_INITIAL;

	//***ИНИЦИАЛИЗАЦИЯ UART***//
	rscs_uart_bus_t * uart0 = rscs_uart_init(RSCS_UART_ID_UART0,
			(RSCS_UART_FLAG_ENABLE_TX + RSCS_UART_FLAG_ENABLE_RX
					+ RSCS_UART_FLAG_BUFFER_RX));
	rscs_uart_set_character_size(uart0, 8);
	rscs_uart_set_baudrate(uart0, 9600);
	rscs_uart_set_stop_bits(uart0, 1);

	FILE * f = rscs_make_uart_stream(uart0);
	stdout = f;

	printf("hello world\n");

	//***ИНИЦИАЛИЗАЦИЯ I2C***//
	rscs_i2c_init();
	rscs_i2c_set_scl_rate(400);

	//***ИНИЦИАЛИЗАЦИЯ TIMESERVICE***//
	rscs_time_init();

	//***ИНИЦИАЛИЗАЦИЯ BMP280***//
	bmp280 = rscs_bmp280_initi2c(RSCS_BMP280_I2C_ADDR_LOW);	//создание дескриптора
	rscs_bmp280_parameters_t bmp280_parameters;
	bmp280_parameters.pressure_oversampling = RSCS_BMP280_OVERSAMPLING_X4;//4		16		измерения на один результат
	bmp280_parameters.temperature_oversampling = RSCS_BMP280_OVERSAMPLING_X2;//1		2		измерение на один результат
	bmp280_parameters.standbytyme = RSCS_BMP280_STANDBYTIME_500US;//0.5ms	62.5ms	время между 2 измерениями
	bmp280_parameters.filter = RSCS_BMP280_FILTER_X16;	//x16	x16		фильтр

	rscs_bmp280_setup(bmp280, &bmp280_parameters);			//запись параметров
	rscs_bmp280_changemode(bmp280, RSCS_BMP280_MODE_NORMAL);//установка режима NORMAL, постоянные измерения

	bmp280_calibration_values = rscs_bmp280_get_calibration_values(bmp280);

	//***ИНИЦИАЛИЗАЦИЯ nRF24L01***//
	nRF24L01_init();

	//FIXME: УБРАТЬ
	//initial_params.zero_pressure = set_zero_pressure();
	//initial_params.zero_pressure = set_zero_pressure();

	//Включение внешних прервыений 0
	EICRA = (1 << ISC01) | (1 << ISC00);		//Прерывание срабатывает при возрастании сигнала
	EIMSK = (1 << INT0);						//Включение прерывания INT0
	sei();										//Глобальное включение прерываний

	while (1) {

		printf("\n-------------------------\nSTEP = %ld\n", step);
		step++;

		//Читаем датчики

		rscs_bmp280_read(bmp280, &raw_data.bmp280_rawpress,
				&raw_data.bmp280_rawtemp);
		rscs_bmp280_calculate(bmp280_calibration_values,
				raw_data.bmp280_rawpress, raw_data.bmp280_rawtemp,
				&TM_package.pressure, &TM_package.temperature);
		TM_package.time = rscs_time_get() / 1016.0;
		TM_package.height = calculate_height(TM_package.pressure,
				initial_params.zero_pressure);



		//Отправляем телеметрию
//		send_package();

		//Отправляем телеметрию пакетами Mavlink
		mavlink_message_t msg;
		mavlink_msg_atmega_pack(1, 1, &msg, TM_package.time, TM_package.pressure,
				TM_package.height, TM_package.temperature, TM_package.state);
		uint8_t buffer[100];
		uint16_t len = mavlink_msg_to_send_buffer(buffer, &msg);

//		while(1)
//		{
//			uint8_t status = nRF24L01_read_status();
//			if ((status & (1 << TX_DS)) || (status & (1 << MAX_RT)))
//				break;
//		}

		printf("here");

//		nRF24L01_clear_TX_FIFO();
		nRF24L01_clear_status(true, true, true);
		nRF24L01_write(buffer, len, true);

		//Отправляем тестовое сообщение
		/*		nRF24L01_clear_TX_FIFO();
		 nRF24L01_clear_status(false, true, true);
		 nRF24L01_write(message, sizeof(message), true);
		 data_register = nRF24L01_read_status();
		 */printf("***\nSTATUS_RX_DR = %d\nSTATUS_TX_DS = %d\nSTATUS_MAX_RT = %d\nSTATUS_RX_P_NO = %d\nSTATUS_TX_FULL = %d\n",
		 (((data_register) & (1 << RX_DR)) >> RX_DR),
		 (((data_register) & (1 << TX_DS)) >> TX_DS),
		 (((data_register) & (1 << MAX_RT)) >> MAX_RT),
		 (((data_register) & (0b111 << RX_P_NO)) >> RX_P_NO),
		 (((data_register) & (1 << TX_FULL))) >> TX_FULL);


//		printf("time = %f s\n", TM_package.time);
//		printf("bmp280_press = %f\n bmp280_temp = %f\n", TM_package.pressure,
//				TM_package.temperature);
//		printf("height = %f\n", TM_package.height);
//
//		//Если по радиоканалу пришли данные, то читаем их
//		memset(RM_input_data, 0x00, sizeof(RM_input_data));
//		RM_input_data_len = get_package(RM_input_data);
//		if (!RM_input_data_len)
//			printf("NO INCOMING TRANSMISSION\n");
//		else {
//			RM_input_data[RM_input_data_len] = 0x00;
//			printf("INCOMING TRANSMISSION: %s\n", RM_input_data);
//		}
//
//		printf("state_parachute = %d\n",
//				(bool) (TM_package.state & (1 << state_parachute)));

		//FIXME: TEST: Получаем команды с UART
//		RX_get = rscs_uart_read_some(uart0, RX_buffer, 1);

		switch (my_stage) {
		case STAGE_INITIAL:

			//Проверяем команду с земли
			if (RX_get) {
				printf("COMMAND: %d\n", RX_buffer[0]);
				if (RX_buffer[0] == STAGE_LOADING_COMMAND)
					my_stage = STAGE_LOADING;
				else
					printf("Got Wrong command\n");
			} else
				printf("There is no data\n");

			break;

		case STAGE_LOADING:

			//Проверяем команду с земли
			if (RX_get) {
				printf("COMMAND: %d\n", RX_buffer[0]);
				if (RX_buffer[0] == STAGE_INITIAL_PARAM_COMMAND)
					my_stage = STAGE_INITIAL_PARAM;
				else
					printf("Got Wrong command\n");
			} else
				printf("There is no data\n");

			break;

		case STAGE_INITIAL_PARAM:
			// Определяем начальное давление
			if (!initial_params.zero_pressure)
				initial_params.zero_pressure = set_zero_pressure();

			//Проверяем команду с Земли
			if (RX_get) {
				printf("COMMAND: %d\n", RX_buffer[0]);
				if (RX_buffer[0] == STAGE_GOING_UP_COMMAND)
					my_stage = STAGE_GOING_UP;
				else
					printf("Got Wrong command\n");
			} else
				printf("There is no data\n");

			break;

		case STAGE_GOING_UP:
			if (check_down_height())
				my_stage = STAGE_GOING_DOWN;

			break;

		case STAGE_GOING_DOWN:

			//Проверяем команду с земли
			if (RX_get) {
				printf("COMMAND: %d\n", RX_buffer[0]);
				printf("DEPLOYMENT_COMMAND: %d\n", DEPLOYMENT_COMMAND);
				if (RX_buffer[0] == DEPLOYMENT_COMMAND)
					deploy_parashute();
				else
					printf("Got Wrong command\n");
			} else
				printf("There is no data\n");

			//if (check_command (RX_buffer, nRF24L01_RX_BUFFER_LEN)) deploy_parashute ();

			//if (check_height ( TM_package.height)) deploy_parashute();

			//if (check_invariable_height()) my_stage = STAGE_ALL_DONE;

			break;

		case STAGE_ALL_DONE:
			break;
		}

		printf("my_stage = %d", my_stage);
		_delay_ms(500);
	}
	return 0;
}

