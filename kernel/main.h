#ifndef MAIN_H_
#define MAIN_H_

#include "frame_buffer.h"
#include "int.h"

typedef struct {
    FrameBuffer *frame_buffer;
    void *memory_map;
    uintn memory_map_size;
    uintn map_descriptor_size;
} EntryParams;

#endif
