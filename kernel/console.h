#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "frame_buffer.h"

typedef struct console {
  int x;
  int y;
  int width;
  int height;
  FrameBuffer *frame_buffer;
} Console;

void console_init(Console *console, FrameBuffer *frame_buffer);
void console_print_char(Console *console, char c);
void console_print_string(Console *console, const char *str);
int console_input(Console *console, char *buffer, int size);

#endif
