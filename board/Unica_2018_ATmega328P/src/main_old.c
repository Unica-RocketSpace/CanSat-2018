/*
 * main_old.c
 *
 *  Created on: 24 марта 2018 г.
 *      Author: developer
 */


int main_old() {
	//***ИНИЦИАЛИЗАЦИЯ UART***//
	rscs_uart_bus_t * uart0 = rscs_uart_init(RSCS_UART_ID_UART0, (RSCS_UART_FLAG_ENABLE_TX + RSCS_UART_FLAG_ENABLE_RX));
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
	bmp280 = rscs_bmp280_initi2c(RSCS_BMP280_I2C_ADDR_LOW);		//создание дескриптора
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



	while(1){
		printf("\n-------------------------\nSTEP = %ld\n", step);
		step++;
		nRF24L01_read_register_address (nRF24L01_RX_ADDR_P0_ADDR, device_address, 5);
		printf("nRF24L01_RX_ADDR_P0_ADDR = 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",	(int)device_address[0],
																					(int)device_address[1],
																					(int)device_address[2],
																					(int)device_address[3],
																					(int)device_address[4]);

		nRF24L01_clear_status(false, true, true);
		data_register = nRF24L01_read_register (nRF24L01_STATUS_ADDR);
		printf("STATUS_RX_DR = %d\nSTATUS_TX_DS = %d\nSTATUS_MAX_RT = %d\nSTATUS_RX_P_NO = %d\nSTATUS_TX_FULL = %d\n",
				(((data_register) & (1 << RX_DR)) >> RX_DR),
				(((data_register) & (1 << TX_DS)) >> TX_DS),
				(((data_register) & (1 << MAX_RT)) >> MAX_RT),
				(((data_register) & (0b111 << RX_P_NO)) >> RX_P_NO),
				(((data_register) & (1 << TX_FULL))) >> TX_FULL);

		//nRF24L01_write (array, 1, false);
		nRF24L01_clear_TX_FIFO();
		nRF24L01_write (message, sizeof(message), false);

		//_delay_ms(300);
		data_register = nRF24L01_read_register (nRF24L01_STATUS_ADDR);
		printf("STATUS_RX_DR = %d\nSTATUS_TX_DS = %d\nSTATUS_MAX_RT = %d\nSTATUS_RX_P_NO = %d\nSTATUS_TX_FULL = %d\n",
				(((data_register) & (1 << RX_DR)) >> RX_DR),
				(((data_register) & (1 << TX_DS)) >> TX_DS),
				(((data_register) & (1 << MAX_RT)) >> MAX_RT),
				(((data_register) & (0b111 << RX_P_NO)) >> RX_P_NO),
				(((data_register) & (1 << TX_FULL))) >> TX_FULL);
		//_delay_ms(400);

		RX_get = nRF24L01_read(RX_buffer, nRF24L01_RX_BUFFER_LEN);
		if (RX_get) printf("%s\n", RX_buffer);
		else printf ("NO Data\n");
		nRF24L01_clear_status(true, false, false);
		nRF24L01_clear_RX_FIFO();
		//_delay_ms(300);

		printf("\n\n\n\n");


		rscs_bmp280_read(bmp280, &raw_data.bmp280_rawpress, &raw_data.bmp280_rawtemp);
		printf("bmp280_rawpress = %u, bmp280_rawtemp = %u\n", (int)raw_data.bmp280_rawpress, (int)raw_data.bmp280_rawtemp);
		rscs_bmp280_calculate(bmp280_calibration_values, raw_data.bmp280_rawpress, raw_data.bmp280_rawtemp, &TM_package.pressure, &TM_package.temperature);
		printf("bmp280_press = %f, bmp280_temp = %f\n", TM_package.pressure, TM_package.temperature);

		calculate_height( TM_package.pressure,  initial_params.zero_pressure);
		printf("height = %f\n", TM_package.height);

		TM_package.time = rscs_time_get() / 1016.0 ;
		printf("rscs_time = %d\n", (int)rscs_time_get());
		printf("time = %f s\n", TM_package.time);

		send_package();

		nRF24L01_read_status();

		_delay_ms(1000);
	}

	return 0;
}
