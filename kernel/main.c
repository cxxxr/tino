#include "main.h"
#include "frame_buffer.h"
#include "asmfunc.h"
#include "serial.h"
#include "gdt.h"
#include "paging.h"

void print_uint64_with_padding(uint64 value, int width)
{
    char buffer[21];
    int i = 20, column = 0;
    for (;;) {
        buffer[--i] = (value % 10) + '0';
        column++;
        value /= 10;
        if (value == 0) break;
    }
    while (width-- > column) {
        buffer[--i] = ' ';
    }
    buffer[20] = '\0';
    serial_write_string(buffer + i);
}

void print_uint64(uint64 value)
{
    print_uint64_with_padding(value, 0);
}

#define print_string serial_write_string
#define print_char serial_write_char

int is_available_memory_type(_EFI_MEMORY_TYPE memory_type)
{
    switch (memory_type) {
    case _EFI_BOOT_SERVICES_CODE:
    case _EFI_BOOT_SERVICES_DATA:
    case _EFI_CONVENTIONAL_MEMORY:
        return 1;
    default:
        return 0;
    }
}

uint8 kernel_stack[1024 * 1024];

void kernel_entry(EntryParams *params)
{
    FrameBuffer *frame_buffer = params->frame_buffer;
    void *memory_map = params->memory_map;
    uintn memory_map_size = params->memory_map_size;
    uintn map_descriptor_size = params->map_descriptor_size;

    init_serial_ports();
    init_gdt();
    init_page_table();

    draw_rectangle(frame_buffer, 0, 0,
                   frame_buffer->horizontal_resolution,
                   frame_buffer->vertical_resolution, 0xffffff);

    print_string("type physical_start virtual_start number_of_pages attribute\n");
    for (uint64 iter = (uint64)memory_map;
         iter < (uint64)(memory_map + memory_map_size);
         iter += map_descriptor_size) {
        _EFI_MEMORY_DESCRIPTOR *desc = (_EFI_MEMORY_DESCRIPTOR*)iter;
        print_uint64_with_padding(desc->type, 4);
        print_char(' ');
        print_uint64_with_padding((uint64)desc->physical_start, 14);
        print_char(' ');
        print_uint64_with_padding((uint64)desc->virtual_start, 13);
        print_char(' ');
        print_uint64_with_padding(desc->number_of_pages, 15);
        print_char(' ');
        print_uint64_with_padding(desc->attribute, 9);
        print_char('\n');
    }

    while (1) __asm__("hlt");
}
