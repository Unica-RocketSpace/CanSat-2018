/*
 * dump.c
 *
 *  Created on: 05 окт. 2016 г.
 *      Author: snork
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <diag/Trace.h>

#include "FreeRTOS.h"
#include "task.h"

#include "drivers/sd/dump.h"
#include "state.h"

static FATFS fs;
static bool fs_mounted = false;

void dump_init(dump_state_t * state, const char * filename)
{
	FRESULT res;
	size_t i = 0;

	// монтируем диск
	if (!fs_mounted )
		do {
			res = f_mount(&fs, "0", 1);
			trace_printf("mount res = %d\n", res);
			i++;
		taskENTER_CRITICAL();
			state_system.SD_state = (uint8_t)res;
		taskEXIT_CRITICAL();
			if (i >= 3)
			{
				trace_printf("mount fails\n");
				abort();
			}
		} while (res != FR_OK);
	fs_mounted = true;

	// выбираем имя файлу
	char fname[15];
	for (i = 0; i < 100; i++)
	{
		fname[0] ='\0';
		sprintf(fname, "0:/%s%u.txt", filename, i);
		FILINFO info;
		if ((res = f_stat(fname, &info)) == FR_NO_FILE)
		{
			// отлично - такого файла нет!
			trace_printf("dump file name '%s' ok\n", fname);
			break;
		}
		else if (res == FR_OK)
		{
			//trace_printf("dump file name '%s' already exists\n", fname);
		}
		else
		{
			trace_printf("dump fname error '%s': err: %d\n", fname, res);
			abort();
		}
	}

	// предполагается что с именем файла мы всетаки определились раз мы тут
	if (FR_OK != (res = f_open(&state->file, fname, FA_WRITE | FA_OPEN_ALWAYS | FA_OPEN_APPEND)))
	{
		trace_printf("open error = %d\n", res);
		abort();
	}
}

void dump(dump_state_t * state, const void * data, size_t datasize)
{
	UINT dummy;
	FRESULT res;
	// пишем на флешку
	if ((res = f_write(&state->file, data, datasize, &dummy)) != FR_OK) {
		state->res = res;
	taskENTER_CRITICAL();
		state_system.SD_state = res;
	taskEXIT_CRITICAL();
	}
//		trace_printf("write error %d\n");

	// было int_fast32_t
	static int32_t sync_counter = 0;
	if (sync_counter >= 10)
	{
		if ((res = f_sync(&state->file)) != FR_OK) {
			state->res = res;
		taskENTER_CRITICAL();
			state_system.SD_state = res;
		taskEXIT_CRITICAL();
		}
//			trace_printf("sync error %d\n");
	   sync_counter = 0;
	}
	sync_counter++;
}



