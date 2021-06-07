#ifndef MEMORY_H_
#define MEMORY_H_

#include "int.h"

typedef struct {
    void *base;
    uintn size;
    uintn descriptor_size;
} MemoryMap;

void init_memory(MemoryMap* map);
void* alloc(uint64 size);

#endif
