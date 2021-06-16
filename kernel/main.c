#include "asmfunc.h"
#include "console.h"
#include "fat.h"
#include "frame_buffer.h"
#include "gdt.h"
#include "main.h"
#include "memory.h"
#include "paging.h"
#include "primitive.h"
#include "serial.h"

uint8 kernel_stack[1024 * 1024];

Fat fat;

bool string_equal(const char *str1, const char *str2) {
  for (int i = 0; str1[i] != '\0' && str2[i] != '\0'; i++) {
    if (str1[i] != str2[i]) return FALSE;
  }
  return TRUE;
}

int execute_command(const char *str) {
  serial_write_string(str);
  serial_write_char('\n');
  if (string_equal(str, "list_dir")) {
    fat_list_root_dir(&fat);
  }
  return 0;
}

void kernel_entry(EntryParams *params) {
  FrameBuffer *frame_buffer = params->frame_buffer;
  MemoryMap memory_map = params->memory_map;

  init_serial_ports();
  init_gdt();
  init_page_table();
  init_memory(&memory_map);

  Console console;
  console_init(&console, frame_buffer);

  fat_init(&fat, params->volume_image);

  char str[128];
  while (1) {
    console_print_string(&console, "% ");
    console_input(&console, str, 128);
    execute_command(str);
  }
}
