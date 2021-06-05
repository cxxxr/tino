#include "main.h"
#include "frame_buffer.h"
#include "asmfunc.h"
#include "serial.h"
#include "gdt.h"
#include "paging.h"

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
    MemoryMap memory_map = params->memory_map;

    init_serial_ports();
    init_gdt();
    init_page_table();

    draw_rectangle(frame_buffer, 0, 0,
                   frame_buffer->horizontal_resolution,
                   frame_buffer->vertical_resolution, 0xffffff);

    print_string("type physical_start virtual_start number_of_pages attribute\n");
    for (uint64 iter = (uint64)memory_map.base;
         iter < (uint64)(memory_map.base + memory_map.size);
         iter += memory_map.descriptor_size) {
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
