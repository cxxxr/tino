#include "console.h"
#include "serial.h"
#include "string.h"

#define FONT_WIDTH 8
#define FONT_HEIGHT 16
#define FONT_COLOR 0xFFFFFF

#define CONSOLE_BG 0x000000

void console_init(Console *console, FrameBuffer *frame_buffer,
                  const char *prompt) {
  console->x = 0;
  console->y = 0;
  console->width = FONT_WIDTH * 80;
  console->height = FONT_HEIGHT * 24;
  console->frame_buffer = frame_buffer;
  string_copy(console->prompt, prompt);

  draw_rectangle(frame_buffer, 0, 0, console->width, console->height,
                 CONSOLE_BG);
}

void console_print_char(Console *console, char c) {
  if (c == '\r' || c == '\n') {
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

void console_print_newline(Console *console) {
  console_print_char(console, '\n');
}

void console_print_line(Console *console, const char *line) {
  draw_rectangle(console->frame_buffer, 0, console->y, console->width,
                 FONT_HEIGHT, CONSOLE_BG);
  int x = 0;
  for (int i = 0; console->prompt[i] != '\0'; i++) {
    draw_char(console->frame_buffer, i * FONT_WIDTH, console->y,
              console->prompt[i], FONT_COLOR);
    x += FONT_WIDTH;
  }
  for (int i = 0; line[i] != '\0'; i++) {
    draw_char(console->frame_buffer, x, console->y, line[i], FONT_COLOR);
    x += FONT_WIDTH;
  }
}

int console_input(Console *console, char *buffer, int size) {
  int i = 0;

  buffer[0] = '\0';

  while (1) {
    console_print_line(console, buffer);
    char c = serial_read_char();
    if (c == '\r') {
      break;
    } else if (c == 127) {
      if (i > 0)
        i--;
    } else {
      buffer[i++] = c;
      if (i == size)
        break;
      buffer[i] = 0;
    }
  }
  buffer[i] = '\0';

  console_print_newline(console);

  return i;
}
