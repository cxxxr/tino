#ifndef ASM_FUN_H_
#define ASM_FUN_H_

#include "int.h"

void write_io_port8(uint16 addr, uint8 data);
uint8 read_io_port8(uint16 addr);
void write_io_port32(uint16 addr, uint32 data);
uint32 read_io_port32(uint16);
void load_gdtr(void *addr);
void set_ds_all(uint16 di);
void set_cs_ss(uint16 cs, uint16 ss);
void set_cr3(uint64 addr);

#endif
