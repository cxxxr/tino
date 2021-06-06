#include "memory.h"
#include "efi.h"
#include "serial.h"

// ページングで扱う最大サイズと合わせて64GiBにする
static const uint64 max_physical_memory_size = 64 * 1024 * 1024 * 1024;
static const uint64 frame_size = 4 * 1024;
static const uint64 memory_map_size = max_physical_memory_size / frame_size;

static uint64 memory_map[memory_map_size];

static void set_bit(uint64 pos, int bit)
{
    uint64 line = pos / 64;
    uint64 column = pos % 64;

    if (bit) {
        memory_map[line] |= ((uint64)1 << column);
    } else {
        memory_map[line] &= ~((uint64)1 << column);
    }
}

static int get_bit(uint64 line, uint64 column)
{
    return (memory_map[line] >> column) & 1;
}

static void mark_free(uint64 start, uint64 size)
{
    for (uint64 i = 0; i < size; i++) {
        set_bit(start + i, 1);
    }
}

static int is_available_memory_type(_EFI_MEMORY_TYPE memory_type)
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

#define KiB(n) ((n) * 1024)

void init_memory(MemoryMap* map)
{
    for (uint64 iter = (uint64)map->base;
         iter < (uint64)(map->base + map->size);
         iter += map->descriptor_size) {
        _EFI_MEMORY_DESCRIPTOR *desc = (_EFI_MEMORY_DESCRIPTOR*)iter;
        if (is_available_memory_type(desc->type)) {
            mark_free(desc->physical_start / KiB(4),
                      desc->number_of_pages);
        }
    }
}
