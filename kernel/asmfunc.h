#ifndef ASM_FUN_H_
#define ASM_FUN_H_

#include "int.h"

void write_io_port8(uint16 addr, uint8 data);
uint8 read_io_port8(uint16 addr);

#endif
