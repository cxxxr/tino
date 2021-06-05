#ifndef SERIAL_H_
#define SERIAL_H_

#include "int.h"

void init_serial_ports(void);
char serial_read_char(void);
void serial_write_char(char a);
void serial_write_string(const char *str);

void print_uint64_with_padding(uint64 value, int width);
void print_uint64(uint64 value);

#define print_string serial_write_string
#define print_char serial_write_char

#endif