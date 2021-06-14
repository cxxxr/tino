#include "memory.h"
#include "efi.h"
#include "serial.h"

static int is_available_memory_type(_EFI_MEMORY_TYPE memory_type) {
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

#if 0
// 物理アドレス0とNULLの区別が付かないため、最後の要素の1ビット目に印をつける
static int is_end(struct page* p)
{
    return (uint64)p->next & 1;
}
#endif

static void set_end(struct page *p) {
  p->next = (struct page *)((uint64)p->next | 1);
}

static uint8 is_first = 1;
static void append_free(uint64 start, uint64 npages) {
  struct page *node = (struct page *)start;
  node->next = head.next;
  node->size = npages * 4096 / sizeof(struct page);

  if (is_first) {
    is_first = 0;
    set_end(node);
  }

  head.next = node;
}

void *alloc(uint64 size) {
  struct page *p;
  struct page *q;

  size = (size + sizeof(struct page) - 1) / sizeof(struct page) + 1;

  print_uint64(size);

  for (p = head.next, q = &head;; q = p, p = p->next) {
    if (p->size >= size) {
      if (p->size == size) {
        q->next = p->next;
      } else {
        p->size -= size;
        p += p->size;
        p->size = size;
      }

      return (void *)(p + 1);
    }
  }
}

#if 0
static void print_free_list(void)
{
    for (struct page *p = head.next; ; p = p->next) {
        print_uint64((uint64)p);
        print_char(' ');
        print_uint64(p->size);
        print_char('\n');
        if (is_end(p)) break;
    }
}
#endif

static void print_memory_map(MemoryMap *map) {
  print_string("type physical_start virtual_start number_of_pages attribute\n");
  for (uint64 iter = (uint64)map->base; iter < (uint64)(map->base + map->size);
       iter += map->descriptor_size) {
    _EFI_MEMORY_DESCRIPTOR *desc = (_EFI_MEMORY_DESCRIPTOR *)iter;
    print_char(is_available_memory_type(desc->type) ? '!' : ' ');
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
}

void init_memory(MemoryMap *map) {
  print_memory_map(map);

  for (uint64 iter = (uint64)map->base; iter < (uint64)(map->base + map->size);
       iter += map->descriptor_size) {
    _EFI_MEMORY_DESCRIPTOR *desc = (_EFI_MEMORY_DESCRIPTOR *)iter;
    if (is_available_memory_type(desc->type)) {
      append_free(desc->physical_start, desc->number_of_pages);
    }
  }

  // print_uint64(sizeof(struct page));
  // print_char('\n');

  // print_free_list();
  // print_string("------------\n");
  // alloc(10);
  // print_free_list();
}
