#ifndef GDT_H_
#define GDT_H_

#include "int.h"

uint64 gdt[3];

void init_gdt(void);

#endif