#ifndef MAIN_H_
#define MAIN_H_

#include "frame_buffer.h"
#include "int.h"
#include "memory.h"

typedef struct {
  FrameBuffer *frame_buffer;
  MemoryMap memory_map;
  void *volume_image;
} EntryParams;

#endif
