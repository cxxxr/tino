#ifndef MAIN_H_
#define MAIN_H_

#include "frame_buffer.h"
#include "int.h"

typedef struct {
    void *base;
    uintn size;
    uintn descriptor_size;
} MemoryMap;

typedef struct {
    FrameBuffer *frame_buffer;
    MemoryMap memory_map;
} EntryParams;

#endif
