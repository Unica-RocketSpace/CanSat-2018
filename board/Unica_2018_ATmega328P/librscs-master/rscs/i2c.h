/*
 * i2c.h
 *
 * Работа с шиной I2C
 */

#ifndef RSCS_I2C_H_
#define RSCS_I2C_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "error.h"

/*	Для использования модуля необходимые базовые знания о том, как ведется обмен данных по шине I2C
 *
 * Сама шина хорошо описана тут:
 * http://easyelectronics.ru/interface-bus-iic-i2c.html
 *
 * Описание аппаратного модуля I2C в микроконтроллерах AVR в переведенном даташите - тут
 * http://www.gaw.ru/html.cgi/txt/doc/micros/avr/arh128/18.htm
 *
 * Примеры использования модуля в конце файла
 */

typedef uint8_t rscs_i2c_addr_t;


// Режим I2C транзакции. Передается вместе с 7мибитным адресом ведомого устройства
typedef enum
{
	rscs_i2c_slaw_write = 0,	// Режим записи. В этом режиме ведомый принимает данные от нас и отвечает ACK-битами
	rscs_i2c_slaw_read = 1,		// Режим чтения. В этом режиме ведомый передает данные, а мы отдаем ему ACK-биты
} rscs_i2c_slaw_mode_t;


// Инициализация i2c.
/* Инциализиует аппаратный модуль TWI (Two Wire Interface, тот же I2C по факту) в микроконтроллере.
   Настраивает тактовую частоту шины на сандартные 100кГц.
   Достаточно вызвать один раз во всей программе. */
void rscs_i2c_init(void);


// Установка частоты линии scl (в кГц)
void rscs_i2c_set_scl_rate(uint32_t f_scl_kHz);


// Создает на i2c шине событие START.
/* Если шина и так находится под нашим контролем - этот вызов создаст событие RESTART.
   Если занять шину не удалось (например линия SCK закорочена на ноль, вернет ошибку RSCS_E_TIMEOUT по таймауту
   Величина таймаута настраивается в конфиге */
rscs_e rscs_i2c_start(void);


// Завершает сессию обмена данными, создавая на шине состояние stop
/* Так же выключает модуль TWI, сбрасывая все его ошибки */
void rscs_i2c_stop(void);


// Отправляет на шину 7мибитный адрес ведомого устройства с битом-флагом чтения/записи и анализирует его ответ - ACK-бит
/* Возвращает ошибки:
 - RSCS_E_NO_DEVICE - в случае, если ведомый не ответил ack-битом
 - RSCS_E_TIMEOUT - в случае, если на линии SCK кто-то слишком долго удерживает 0 и передача байта длится слишком долго */
rscs_e rscs_i2c_send_slaw(uint8_t slave_addr, rscs_i2c_slaw_mode_t mode);


// Передача данных ведомому устройству и анализ его ответа - ACK-бита
/* Эту функцию имеет смысл вызывать, только если текущая транзакция начата в режиме записи
   Записывает всего один байт, передаваемый в аргументе `byte`

   Возвращает ошибки:
 - RSCS_E_NO_DEVICE - в случае, если ведомый не ответил ack-битом
 - RSCS_E_TIMEOUT - в случае, если на линии SCK кто-то слишком долго удерживает 0 и передача байта длится слишком долго */
rscs_e rscs_i2c_write_byte(uint8_t byte);


// Передача данных ведомому устройству и анализ его ответа - ACK-бита
/* Эту функцию имеет смысл вызывать, только если текущая транзакция начата в режиме записи
   Записывает data_size байт из памяти по указателю data_ptr на устройство.

   Возвращает ошибки:
 - RSCS_E_NO_DEVICE - в случае, если ведомый не ответил ack-битом
 - RSCS_E_TIMEOUT - в случае, если на линии SCK кто-то слишком долго удерживает 0 и передача байта длится слишком долго */
rscs_e rscs_i2c_write(const void * data_ptr, size_t data_size);


// Получение данных от ведомого устройства
/* Эту функцию имеет смысл вызывать, только если текущая транзакция начата в режиме чтения

   Получает data_size байт от ведомого устройства и записывает по указанному указателю data_ptr.
   аргумент NACK_at_end позволяет управлять подтверждением получения байта от ведомого.
   если этот аргумент установлен в true, то после получения последнего байта ведомому будет передан
   Not ACKnowledge, что означает завершение сеанса чтения.

   Возвращает ошибки:
 - RSCS_E_TIMEOUT - в случае, если на линии SCK кто-то слишком долго удерживает 0 и передача байта длится слишком долго */
rscs_e rscs_i2c_read(void * data_ptr, size_t data_size, bool NACK_at_end);



// ========================================================================
// Примеры использования модуля:
// ========================================================================

//Передача данных ведомому
/*
	// Сперва нужно иницилизировать шину (это нужно делать один раз во всей программе)
	// вызвав rscs_i2c_init();

	// опишем передачу массива данных в виде функции
	rscs_e i2c_write_some_data(void)
	{
		rscs_e error = RSCS_E_NONE; // пока что считаем, что у нас нет ошибок

		// Сперва нужно начать транзакцию
		error = rscs_i2c_start();
		// анализируем ошибку - получись ли это у нас?
		if (error != RSCS_E_NONE)
			// если не получилось, сразу завершаемся
			return error;

		// передать адрес ведомого и режим транзакции, в нашем случае это будет режим записи
		error = rscs_i2c_send_slaw(0xAA, rscs_i2c_slaw_write);
		// анализируем ошибку
		if (error != RSCS_E_NONE)
		{
			// если что-то пошло не так, сразу завершаемся.
			// однако, у нас уже есть i2c транзакция, которую просто необходимо завершить вызовом i2c_stop
			// поэтому переходим в конец функции при помощи оператора goto
			goto end;
		}

		// передадим ведомому байт 0xFF
		error = rscs_i2c_write_byte(0xFF);
		if (error != RSCS_E_NONE)
			goto end;

		// передадим ведомому еще и массив байт
		uint8_t data[] = {0xFF, 0xFF, 0xFF};
		error = rscs_i2c_write_byte(data, sizoef(data));
		if (error != RSCS_E_NONE)
			goto end;

		// все, завершаем транзакцию

	end:
		rscs_i2c_stop();
		return error;
	}
*/


// Приём данных от ведомого
/*
	// Сперва нужно иницилизировать шину (это нужно делать один раз во всей программе)
	// вызвав rscs_i2c_init();

	// опишем приём массива в виде функции, которая читает данные по указателям, которые передаются аргументами
	rscs_e i2c_read_some_data(uint8_t * someBytePtr, uint16_t * someWordPtr, uint8_t * someArrayPtr, size_t someArray_size)
	{
		rscs_e error = RSCS_E_NONE; // пока что считаем, что у нас нет ошибок

		// Сперва нужно начать транзакцию
		error = rscs_i2c_start();
		// анализируем ошибку - получись ли это у нас?
		if (error != RSCS_E_NONE)
			// если не получилось, сразу завершаемся
			return error;

		// передать адрес ведомого и режим транзакции, в нашем случае это будет режим чтения
		error = rscs_i2c_send_slaw(0xAA, rscs_i2c_slaw_read);
		// анализируем ошибку
		if (error != RSCS_E_NONE)
		{
			// если что-то пошло не так, сразу завершаемся.
			// однако, у нас уже есть i2c транзакция, , которую просто необходимо завершить вызовом i2c_stop
			// поэтому переходим в конец функции при помощи оператора goto
			goto end;
		}

		// читаем от ведомого первый байт, который у нас запросили
		// функция автоматически ответит ACK на каждый принятый байт, кроме последнего
		// модуль I2C в AVR микроконтроллерах устроен так, что на последний принятый байт (перед вызвом i2c_stop)
		// обязательно нужно ответить NACK, а не ACK. Этот самый последний ACK бит управляется третьим аргументом
		// функции i2c_read. Если вы планирует что-то еще читать в рамках транзакции, следует указать этот аргумент == false
		// если это последняя операция чтения в транзакции, нужно ОБЯЗАТЕЛЬНо указать его == true
		error = rscs_i2c_read(someBytePtr, 1, false);
		if (error != RSCS_E_NONE)
			goto end;

		// теперь читаем двухбайтовое слово
		error = rscs_i2c_read(someWordPtr, 2, false);
		if (error != RSCS_E_NONE)
			goto end;


		// теперь читаем массив байт указанного размера
		// при этом говорим, что это последнее чтение в этой транзакции, указав true в последнем аргументе
		error = rscs_i2c_read(someArrayPtr, someArray_size, true);
		if (error != RSCS_E_NONE)
			goto end;

		// все, завершаем транзакцию

	end:
		rscs_i2c_stop();
		return error;
	}
*/


// Комбинированная передача и приём данных в одной транзакции
// очень часто, ведомые i2c устройства обеспечивают регистровый доступ к данным.
// таким образом - первый переданный от нас байт, ведомый воспринимает как адрес регистра, в который нужно записать последующие данные
// в режиме записи с этим работать просто - просто добавляем перед массивом передаваемых данных еще один байт
// в режиме чтения же немного сложнее, так как требуется передать данные, чего мы не можем сделать в режиме чтения.
// следующий пример показывает как работать в такой ситуации
/*
	// Сперва нужно иницилизировать шину (это нужно делать один раз во всей программе)
	// вызвав rscs_i2c_init();

	// опишем передачу массива данных в виде функции
	// функция принимает два аргумента:
	// - reg_addr - адрес регистра (на ведомом устройстве), из которого нужно прочесть данные -
	// - reg_value - указатель на двухбайтную переменную, в которую нужно записать прочитанное значение
	rscs_e i2c_read_reg(uint8_t reg_addr, uint16_t * reg_value)
	{
		rscs_e error = RSCS_E_NONE; // пока что считаем, что у нас нет ошибок

		// Сперва нужно начать транзакцию
		error = rscs_i2c_start();
		// анализируем ошибку - получись ли это у нас?
		if (error != RSCS_E_NONE)
			// если не получилось, сразу завершаемся
			return error;

		// передать адрес ведомого и режим транзакции, пока что это будет режим записи
		// чтобы мы могли передать адрес нужного регистра
		error = rscs_i2c_send_slaw(0xAA, rscs_i2c_slaw_write);
		// анализируем ошибку
		if (error != RSCS_E_NONE)
		{
			// если что-то пошло не так, сразу завершаемся.
			// однако, у нас уже есть i2c транзакция, которую просто необходимо завершить вызовом i2c_stop
			// поэтому переходим в конец функции при помощи оператора goto
			goto end;
		}

		// передадим ведомому байт адрес интересующего регистра
		error = rscs_i2c_write_byte(0xFF);
		if (error != RSCS_E_NONE)
			goto end;

		// теперь создаем на шине событие RESTART
		// это какбы начало новой транзакции, без завершения предидущей
		// делается вполне очевидным образом - повторным вызовом rscs_i2c_start
		error = rscs_i2c_start();
		if (error != RSCS_E_NONE)
			goto end;

		// снова передаем адрес ведомого, но уже в режиме чтения!
		error = rscs_i2c_send_slaw(0xAA, rscs_i2c_slaw_read);
		if (error != RSCS_E_NONE)
			goto end;

		// и читаем нужные данные, третьим аргументом показывая, что мы больше не будем ничего читать
		// (смотри пример на чтение для более подробных комментариев про третий параметр
		error = rscs_i2c_read(reg_value, 2, true);
		if (error != RSCS_E_NONE)
			goto end;

		// все, завершаем транзакцию

	end:
		rscs_i2c_stop();
		return error;
	}
*/



#endif /* RSCS_I2C_H_ */
