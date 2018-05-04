/*
 * dynamic_unit.c
 *
 *  Created on: 11 нояб. 2017 г.
 *      Author: developer
 */
#include "stdbool.h"
#include <math.h>

#include <stm32f4xx_hal.h>

#include <FreeRTOS.h>
#include "task.h"

#include "dynamic_unit.h"
#include "state.h"
#include "quaternion.h"

#define STEP_DIVIDER		2

#define DRV8855_STEP_PIN	GPIO_PIN_13	//C3
#define DRV8855_STEP_PORT	GPIOB
#define DRV8855_DIR_PIN		GPIO_PIN_14	//A4
#define DRV8855_DIR_PORT	GPIOB
#define DRV8855_MODE0_PIN	GPIO_PIN_6	//D2
#define DRV8855_MODE0_PORT	GPIOC
#define DRV8855_MODE1_PIN	GPIO_PIN_8	//C11
#define DRV8855_MODE1_PORT	GPIOC
#define DRV8855_MODE2_PIN	GPIO_PIN_9	//A10
#define DRV8855_MODE2_PORT	GPIOC
#define DRV8855_nFAULT_PIN	GPIO_PIN_10	//B11
#define DRV8855_nFAULT_PORT	GPIOB

#define TARGET_X			100
#define TARGET_Y			100



void step_engine_init () {

	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();

	GPIO_InitTypeDef DRV8825_pins;

	/*Pin STEP*/
	DRV8825_pins.Pin = DRV8855_STEP_PIN;
	DRV8825_pins.Mode = GPIO_MODE_OUTPUT_PP;
	DRV8825_pins.Pull = GPIO_NOPULL;
	DRV8825_pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(DRV8855_STEP_PORT, &DRV8825_pins);

	/*Pin DIR*/
	DRV8825_pins.Pin = DRV8855_DIR_PIN;
	DRV8825_pins.Mode = GPIO_MODE_OUTPUT_PP;
	DRV8825_pins.Pull = GPIO_NOPULL;
	DRV8825_pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(DRV8855_DIR_PORT, &DRV8825_pins);

	/*Pin MODE0*/
	DRV8825_pins.Pin = DRV8855_MODE0_PIN;
	DRV8825_pins.Mode = GPIO_MODE_OUTPUT_PP;
	DRV8825_pins.Pull = GPIO_NOPULL;
	DRV8825_pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(DRV8855_MODE0_PORT, &DRV8825_pins);
	/*Pin MODE1*/
	DRV8825_pins.Pin = DRV8855_MODE1_PIN;
	DRV8825_pins.Mode = GPIO_MODE_OUTPUT_PP;
	DRV8825_pins.Pull = GPIO_NOPULL;
	DRV8825_pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(DRV8855_MODE1_PORT, &DRV8825_pins);
	/*Pin MODE2*/
	DRV8825_pins.Pin = DRV8855_MODE2_PIN;
	DRV8825_pins.Mode = GPIO_MODE_OUTPUT_PP;
	DRV8825_pins.Pull = GPIO_NOPULL;
	DRV8825_pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(DRV8855_MODE2_PORT, &DRV8825_pins);

	/*Pin nFAULT*/
	DRV8825_pins.Pin = DRV8855_nFAULT_PIN;
	DRV8825_pins.Mode = GPIO_MODE_INPUT;
	DRV8825_pins.Pull = GPIO_NOPULL;
	DRV8825_pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(DRV8855_nFAULT_PORT, &DRV8825_pins);

	//TODO:Написать определение MODE-ов с учетом STEP_DIVIDER
	//DRV8855_MODE0_PORT->BSRR = 0xFF;
	HAL_GPIO_WritePin(DRV8855_MODE0_PORT, DRV8855_MODE0_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DRV8855_MODE1_PORT, DRV8855_MODE1_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DRV8855_MODE2_PORT, DRV8855_MODE2_PIN, GPIO_PIN_SET);

}



void calculate_angles () {

	float quat_ISC_RSC[4] = {0, 0, 0, 0};		//	кватернион для перехода из инерциальной системы в связанную
	float target[3] = {0, 0, 0};				//	направляющий вектор цели
	float target_RSC[3] = {0, 0, 0};			//	направляющий вектор цели в ССК

	//TODO: CHOOSE WHAT DATA TO USE: IMU OR GPS
	target[0] = TARGET_X - stateIMU_isc.coordinates[0];
	target[1] = TARGET_Y - stateIMU_isc.coordinates[1];
	target[2] = - stateIMU_isc.coordinates[2];

	quat_invert(stateIMU_isc.quaternion, quat_ISC_RSC);		//	получаем кватернион ИСК->ССК
	vect_rotate(target, quat_ISC_RSC, target_RSC);	//	получаем вектор цели в ССК

	stateCamera_orient.step_engine_pos = atan(target_RSC[0] / target_RSC[1]);
	stateCamera_orient.servo_pos = atan(target_RSC[2] / sqrt(pow(target_RSC[0], 2) + pow(target_RSC[1], 2)));
}


/*error rotate_step_engine () {

	if (HAL_GPIO_ReadPin(DRV8855_nFAULT_PORT, DRV8855_nFAULT_PIN) == 0) return driver_overheat;

	float STEP_DEGREES = stateCamera_orient.step_engine_pos - stateCamera_orient_prev.step_engine_pos;

	rotate_step_engine_by_angles(&STEP_DEGREES);

	return no_error;
}*/

void rotate_step_engine_by_angles (float * angles, bool direction) {

	float STEP_DEGREES = *angles;

	if (direction) HAL_GPIO_WritePin(DRV8855_DIR_PORT, DRV8855_DIR_PIN, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(DRV8855_DIR_PORT, DRV8855_DIR_PIN, GPIO_PIN_RESET);

	float STEP_TERNS = STEP_DEGREES/ (M_PI *2) * 200 * 2/* * pow(2, STEP_DIVIDER)*/;
	printf("STEP_TERNS = %f\r\n", STEP_TERNS );
	for(int i = 0; i < (int)round(STEP_TERNS); i++) {
		HAL_GPIO_WritePin(DRV8855_STEP_PORT, DRV8855_STEP_PIN, GPIO_PIN_SET);
		//HAL_Delay(1);
		for(int j = 0; j < 2000; j++){}			//Таймер
		HAL_GPIO_WritePin(DRV8855_STEP_PORT, DRV8855_STEP_PIN, GPIO_PIN_RESET);
		//HAL_Delay(1);
		for(int j = 0; j < 2000; j++){}			//Таймер
	}

}

uint8_t data = 0;

void MOTOR_task() {

	//Инициализация UART
	usart_motor.Instance = USART1;
	usart_motor.Init.BaudRate = 9600;
	usart_motor.Init.WordLength = UART_WORDLENGTH_8B;
	usart_motor.Init.StopBits = UART_STOPBITS_1;
	usart_motor.Init.Parity = UART_PARITY_NONE;
	usart_motor.Init.Mode = UART_MODE_TX_RX;

	uint8_t error = HAL_USART_Init(&usart_motor);
	printf("error = %d", (int)error);
//	HAL_USART_Transmit(&usart_motor, &error, 1, 0xFF);


	//Инициализация драйвера ШД
	step_engine_init();
	float angle = 100*(M_PI / 4);



//	HAL_USART_Receive_IT(&usart_motor, &data, sizeof(data));
	const TickType_t _delay = 500 / portTICK_RATE_MS;

	for(;;) {
		//while(((usart_motor.Instance->SR) & (1 << 5)) == 0){}
		//printf("data: %lu", usart_motor.Instance->DR);
//		printf("123\n");

		rotate_step_engine_by_angles(&angle, true);
		vTaskDelay(_delay);
		rotate_step_engine_by_angles(&angle, false);
	}

}

//void HAL_USART_RxCpltCallback(USART_HandleTypeDef * husart) {
//
//	if(husart->Instance == USART1) {
//		printf("%d\n", data);
//		HAL_USART_Receive_IT(&usart_motor, &data, sizeof(data));
//	}
//	else {}
//}
//
//
//void USART1_IRQHandler(){
//
//
//}




