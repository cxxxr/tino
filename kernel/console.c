#include "console.h"
#include "serial.h"

#define FONT_WIDTH 8
#define FONT_HEIGHT 16
#define FONT_COLOR 0xFFFFFF

#define CONSOLE_BG 0x000000

void console_init(Console *console, FrameBuffer *frame_buffer) {
  console->x = 0;
  console->y = 0;
  console->width = FONT_WIDTH * 80;
  console->height = FONT_HEIGHT * 24;
  console->frame_buffer = frame_buffer;

  draw_rectangle(frame_buffer, 0, 0, console->width, console->height,
                 CONSOLE_BG);
}

void console_print_char(Console *console, char c) {
  if (c == '\r') {
    console->x = 0;
    console->y += FONT_HEIGHT;
  } else {
    draw_char(console->frame_buffer, console->x, console->y, c, FONT_COLOR);
    console->x += FONT_WIDTH;
  }
}

void console_print_string(Console *console, const char *str) {
  for (int i = 0; str[i]; i++) {
    console_print_char(console, str[i]);
  }
}

int console_input(Console *console, char *buffer, int size) {
  int i = 0;

  while (1) {
    char c = serial_read_char();
    console_print_char(console, c);
    if (c == '\r') {
      break;
    }
    buffer[i++] = c;
    if (i == size)
      break;
  }

  return i;
}
