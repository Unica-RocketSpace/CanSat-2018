/*
 * radio-module.h

 *
 *  Created on: 09 дек. 2017 г.
 *      Author: developer
 */


#ifndef RADIO_MODULE_H_
#define RADIO_MODULE_H_

void send_package();

//Возвращает длину пакта
uint8_t get_package(void * RX_buffer);

#endif /* RADIO_MODULE_H_ */
