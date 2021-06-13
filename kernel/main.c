#include "main.h"
#include "frame_buffer.h"
#include "asmfunc.h"
#include "serial.h"
#include "gdt.h"
#include "paging.h"
#include "memory.h"
#include "console.h"
#include "fat.h"

uint8 kernel_stack[1024 * 1024];

void kernel_entry(EntryParams *params)
{
    FrameBuffer *frame_buffer = params->frame_buffer;
    MemoryMap memory_map = params->memory_map;

    init_serial_ports();
    init_gdt();
    init_page_table();
    init_memory(&memory_map);

    Console console;
    console_init(&console, frame_buffer);

    Fat fat;
    fat_init(&fat, params->volume_image);

    fat_list(&fat);

    while (1) __asm__("hlt");

    // char str[128];
    // while (1) {
    //     console_input(&console, str, 128);
    //     print_string(str);
    //     print_char('\n');
    // }
}
