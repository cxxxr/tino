#ifndef STRING_H_
#define STRING_H_

#include "primitive.h"
#include "int.h"

bool string_equal(const char *str1, const char *str2);
int string_copy(char *dst, const char *src);
void copy_memory(void *destination, void *source, uint64 size);
void set_memory(void *buffer, uint64 size, uint8 value);

#endif
