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

//***ИНИЦИАЛИЗАЦИЯ STATE***//
TM_package_t		TM_package;
initial_params_t	initial_params;
raw_data_t			raw_data;
const rscs_bmp280_calibration_values_t * bmp280_calibration_values;

uint8_t data_register;

uint8_t input_data[nRF24L01_RX_BUFFER_LEN];	//FIXME: TEST
uint8_t input_data_len;
char	message[] = "Hello";
uint8_t device_address[5];
long	step = 0;
bool	RX_get = 0;

uint8_t write_buffer = 0;


uint8_t RX_buffer[nRF24L01_RX_BUFFER_LEN] = {0};


uint8_t nRF24L01_RX_BUFFER[nRF24L01_RX_BUFFER_LEN];

typedef enum
{
	STAGE_INITIAL = 0,		// Состояние, в котором инициализируем все
	STAGE_LOADING = 1,		// Состояние в котором мы мы ничего не делаем и ждем команды
	STAGE_INITIAL_PARAM = 2,// Состояни, в котором мы определяем начальное давление
	STAGE_GOING_UP = 3, 	// Состояние, в котором мы летим вверх
	STAGE_GOING_DOWN = 4, 	// Состояние, в котором мы летим вниз
	STAGE_ALL_DONE = 5, 	// Мы все сделали, отдыхваем
} state_t ;


int main()
{
	state_t my_stage = STAGE_INITIAL;

	//***ИНИЦИАЛИЗАЦИЯ UART***//
	rscs_uart_bus_t * uart0 = rscs_uart_init(RSCS_UART_ID_UART0, (RSCS_UART_FLAG_ENABLE_TX + RSCS_UART_FLAG_ENABLE_RX + RSCS_UART_FLAG_BUFFER_RX));
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
	bmp280 = rscs_bmp280_initi2c(RSCS_BMP280_I2C_ADDR_LOW);						//создание дескриптора
	rscs_bmp280_parameters_t bmp280_parameters;
	bmp280_parameters.pressure_oversampling = RSCS_BMP280_OVERSAMPLING_X4;		//4		16		измерения на один результат
	bmp280_parameters.temperature_oversampling = RSCS_BMP280_OVERSAMPLING_X2;	//1		2		измерение на один результат
	bmp280_parameters.standbytyme = RSCS_BMP280_STANDBYTIME_500US;				//0.5ms	62.5ms	время между 2 измерениями
	bmp280_parameters.filter = RSCS_BMP280_FILTER_X16;							//x16	x16		фильтр

	rscs_bmp280_setup(bmp280, &bmp280_parameters);								//запись параметров
	rscs_bmp280_changemode(bmp280, RSCS_BMP280_MODE_NORMAL);					//установка режима NORMAL, постоянные измерения

	bmp280_calibration_values = rscs_bmp280_get_calibration_values(bmp280);


	//***ИНИЦИАЛИЗАЦИЯ nRF24L01***//
	nRF24L01_init();

	//Включение внешних прервыений 0
	EICRA = (1 << ISC01) | (1 << ISC00); 		//Прерывание срабатывает при возрастании сигнала
	EIMSK = (1 << INT0);		//Включение прерывания INT0
	sei();		//Глобальное включение прерываний

	while(1)
	{

		//printf("\n-------------------------\nSTEP = %ld\n", step);
		step++;

		// Читаем датчики

		rscs_bmp280_read(bmp280, &raw_data.bmp280_rawpress, &raw_data.bmp280_rawtemp);
		rscs_bmp280_calculate(bmp280_calibration_values, raw_data.bmp280_rawpress, raw_data.bmp280_rawtemp, &TM_package.pressure, &TM_package.temperature);
		TM_package.time = rscs_time_get() / 1016.0 ;
		TM_package.height = calculate_height( TM_package.pressure,  initial_params.zero_pressure);


		// отправляем телеметрию
		//send_package();

		//printf("time = %f s\n", TM_package.time);
		//printf("bmp280_press = %f\n bmp280_temp = %f\n", TM_package.pressure, TM_package.temperature);
		//printf("height = %f\n", TM_package.height);


		//FIXME: TEST: Ведем себя как послушная приемная станция
		data_register = nRF24L01_read_register (nRF24L01_STATUS_ADDR);
		printf("***\nSTATUS_RX_DR = %d\nSTATUS_TX_DS = %d\nSTATUS_MAX_RT = %d\nSTATUS_RX_P_NO = %d\nSTATUS_TX_FULL = %d\n",
				(((data_register) & (1 << RX_DR)) >> RX_DR),
				(((data_register) & (1 << TX_DS)) >> TX_DS),
				(((data_register) & (1 << MAX_RT)) >> MAX_RT),
				(((data_register) & (0b111 << RX_P_NO)) >> RX_P_NO),
				(((data_register) & (1 << TX_FULL))) >> TX_FULL);
		_delay_ms(10);

		memset(input_data, 0x00, sizeof(input_data));
		input_data_len = 0x00;

//		uint8_t command = nRF24L01_R_RX_PL_WID;
//		rscs_spi_write(&command, 1);
//		rscs_spi_read(&input_data_len, 1, 0xFF);

		input_data_len = get_package(input_data);
		if (!input_data_len) printf("NO INCOMING TRANSMISSION\n");
		else
		{
			printf("DATA_LENGHT: %d\n", input_data_len);
			printf("INCOMING TRANSMISSION: %s\n", input_data);
		}


		//FIXME: TEST: (закоментированно)
		//получаем ответ
		//RX_get = nRF24L01_read(RX_buffer, nRF24L01_RX_BUFFER_LEN);
		//nRF24L01_clear_RX_FIFO();
		//nRF24L01_clear_status(true, false, false);

		//printf("state_parachute = %d\n", (TM_package.state | (1 << state_parachute)));

		//FIXME: TEST: Получаем команды с UART
		RX_get = rscs_uart_read_some(uart0, RX_buffer, 1);



		switch (my_stage)
		{
		case STAGE_INITIAL:

			//Проверяем команду с земли
			if (RX_get)
			{
				//printf("COMMAND: %d\n", RX_buffer[0]);
				if (RX_buffer[0] == STAGE_LOADING_COMMAND) my_stage = STAGE_LOADING;
				//else printf ("Got Wrong command\n");
			}
			//else printf ("There is no data\n");

			break;

		case STAGE_LOADING:

			//Проверяем команду с земли
			if (RX_get)
			{
				//printf("COMMAND: %d\n", RX_buffer[0]);
				if (RX_buffer[0] == STAGE_INITIAL_PARAM_COMMAND) my_stage = STAGE_INITIAL_PARAM;
				//else printf ("Got Wrong command\n");
			}
			//else printf ("There is no data\n");

			break;

		case STAGE_INITIAL_PARAM:
			// Определяем начальное давление
			if (!initial_params.zero_pressure)initial_params.zero_pressure = set_zero_pressure();

			//Проверяем команду с земли
			if (RX_get)
			{
				//printf("COMMAND: %d\n", RX_buffer[0]);
				if (RX_buffer[0] == STAGE_GOING_UP_COMMAND) my_stage = STAGE_GOING_UP;
				//else printf ("Got Wrong command\n");
			}
			//else printf ("There is no data\n");


			break;

		case STAGE_GOING_UP:
			if (check_down_height()) my_stage = STAGE_GOING_DOWN;

			break;

		case STAGE_GOING_DOWN:
			if (check_command (RX_buffer, nRF24L01_RX_BUFFER_LEN)) deploy_parashute ();

			if (check_height ( TM_package.height)) deploy_parashute();

			if (check_invariable_height()) my_stage = STAGE_ALL_DONE;

			break;

		case STAGE_ALL_DONE:
			break;
		}


		//printf("my_stage = %d", my_stage);
		//_delay_ms(1000);
	}
	return 0;
}

