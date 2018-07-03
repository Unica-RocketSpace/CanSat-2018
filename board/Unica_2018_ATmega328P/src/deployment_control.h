/*
 * deployment_control.h
 *
 *  Created on: 22 дек. 2017 г.
 *      Author: developer
 */

#ifndef DEPLOYMENT_CONTROL_H_
#define DEPLOYMENT_CONTROL_H_

#include <stdbool.h>

#define MIN_HEIGHT 					250.0f
#define DEPLOYMENT_COMMAND			13
#define INVARIABLE_DELTA_HEIGHT		0.01f
#define FALLING_DELTA_HEIGHT		0.01f

#define	STAGE_LOADING_COMMAND   	1
#define	STAGE_INITIAL_PARAM_COMMAND 2
#define	STAGE_GOING_UP_COMMAND   	3
#define	STAGE_GOING_DOWN_COMMAND   	4

#define ENGINE_DDR			DDRB
#define ENGINE_PORT			PORTB
#define ENGINE_PIN			0

/*
 * Функция определяет начальное давление
 * Возвращает давление в паскалях
 */
float set_zero_pressure();

/*
 * Функция считает высоту по давлению
 * Параметры:	pressure - текущее давление
 * 				zero_pressure - нулевое давление
 * Возвращает высоту в метрах
 */
float calculate_height(float pressure, float zero_pressure);

/*
 * Функция сравнивает текущее значение высоты со значанием MIN_HEIGHT
 * Параметры: height - значение высоты
 * Возвращает true - если height <= MIN_HEIGHT, в противном случае возвращает false
 */
bool check_height (float height);

/*
 * Функция сравнивает command с командой 13 (команда на открытие паращюта)
 * Параметры:	command - команда
 * 				command_len - длина команды
 * Возвращает true  если команда подходит, false если не подходит
 */
bool check_command (uint8_t * command, uint8_t command_len);

/*
 * Функция открывает парашют
 */
void deploy_parashute ();

/*
 * Функция определяет падение высоты
 * Возвращает true если фиксируется падение высоты, else если высота не падает
 */
bool check_down_height();

/*
 * Функция определяет неизменность высоты
 * Возвращает true если фиксируется неизменность высоты, else если высота изменяется
 */
bool check_invariable_height();


#endif /* DEPLOYMENT_CONTROL_H_ */
