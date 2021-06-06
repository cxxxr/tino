#include "memory.h"
#include "efi.h"
#include "serial.h"





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



struct page {
    struct page *next;
    uint64 size;
};

static struct page head;

// 物理アドレス0とNULLの区別が付かないため、最後の要素の1ビット目に印をつける
static int is_end(struct page* p)
{
    return (uint64)p->next & 1;
}

static void set_end(struct page* p)
{
    p->next = (struct page*)((uint64)p->next | 1);
}

static uint8 is_first = 1;
static void append_free(uint64 start, uint64 size)
{
    struct page *node = (struct page *)start;
    node->next = head.next;
    node->size = size * 4096; // REVIEW: sizeはヘッダ分減らさなくて良いのか

    if (is_first) {
        is_first = 0;
        set_end(node);
    }

    head.next = node;
}

void print_memory(void)
{
    for (struct page *p = head.next; ; p = p->next) {
        print_uint64((uint64)p);
        print_char(' ');
        print_uint64(p->size);
        print_char(' ');
        print_uint64(p->size / 4096);
        print_char('\n');
        if (is_end(p)) break;
    }
}

void init_memory(MemoryMap* map)
{
    for (uint64 iter = (uint64)map->base;
         iter < (uint64)(map->base + map->size);
         iter += map->descriptor_size) {
        _EFI_MEMORY_DESCRIPTOR *desc = (_EFI_MEMORY_DESCRIPTOR*)iter;
        if (is_available_memory_type(desc->type)) {
            append_free(desc->physical_start,
                        desc->number_of_pages);
        }
    }

    print_memory();
}
