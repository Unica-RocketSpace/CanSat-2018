/*
 * dump.h
 *
 *  Created on: 05 окт. 2016 г.
 *      Author: snork
 */

#ifndef DUMP_H_
#define DUMP_H_

#include <stddef.h>
#include <drivers/sd/fatfs/ff.h>

typedef struct
{
   FIL file;
   FRESULT res;
} dump_state_t;


void dump_init(dump_state_t * state, const char * filename);
void dump(dump_state_t * state, const void * data, size_t datasize);



#endif /* DUMP_H_ */
