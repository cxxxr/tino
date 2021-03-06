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
#include "string.h"
#include "elf.h"

uint8 kernel_stack[1024 * 1024];

Fat fat;
Console console;

bool is_elf_ident(const unsigned char *ident) {
  return (ident[0] == 0x7f &&
          ident[1] == 'E' &&
          ident[2] == 'L' &&
          ident[3] == 'F');
}

int execute_file(File *fp) {
  Elf64_Ehdr *ehdr = (Elf64_Ehdr *)fp->buffer;
  if (!is_elf_ident(ehdr->e_ident)) {
    console_print_string(&console, "Invalid file\n");
    return -1;
  }
  typedef int MainFunc(void);
  return ((MainFunc *)(fp->buffer + ehdr->e_entry))();
}

int execute_command(const char *str) {
  serial_write_string(str);
  serial_write_char('\n');
  if (string_equal(str, "list_dir")) {
    fat_list_root_dir(&fat);
  } else {
    // strが長すぎると転ける
    File *fp = open_file(&fat, string_to_filename(str));
    if (fp == NULL) {
      console_print_string(&console, str);
      console_print_string(&console, " does not exist\n");
    } else {
      int result = execute_file(fp);
      close_file(fp);

      print_uint64(result);
      print_char('\n');
    }
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

  console_init(&console, frame_buffer, "% ");

  fat_init(&fat, params->volume_image);

  char str[128];
  while (1) {
    console_input(&console, str, 128);
    execute_command(str);
  }
}
