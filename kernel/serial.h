#ifndef SERIAL_H_
#define SERIAL_H_

#include "int.h"

void init_serial_ports();
char serial_read_char();
void serial_write_char(char a);
void serial_write_string(const char *str);

#endif