#include "paging.h"
#include "asmfunc.h"
#include "int.h"

#define SIZE 512
#define PDP_N 64

#define KiB(n) ((n)*1024)
#define MiB(n) ((n)*1024 * 1024)

_Alignas(KiB(4)) static uint64 pml4_table[SIZE];
_Alignas(KiB(4)) static uint64 pdp_table[SIZE];
_Alignas(KiB(4)) static uint64 page_directory[PDP_N][SIZE];

// https://wiki.osdev.org/Paging

#define PAGE_DIRECTORY_RW (1 << 1)
#define PAGE_DIRECTORY_PRESENT (1 << 0)
#define PAGE_DIRECTORY_PAGE_SIZE                                               \
  (1 << 7) // 1なら2MiB, 0なら4KiB単位で扱うようになる

void init_page_table(void) {
  const int flags = (PAGE_DIRECTORY_RW | PAGE_DIRECTORY_PRESENT);

  pml4_table[0] = (uint64)pdp_table | flags;

  uint64 addr = 0;
  for (int pdp_i = 0; pdp_i < PDP_N; pdp_i++) {
    pdp_table[pdp_i] = (uint64)&page_directory[pdp_i] | flags;
    for (int pd_i = 0; pd_i < SIZE; pd_i++) {
      page_directory[pdp_i][pd_i] = addr | (flags | PAGE_DIRECTORY_PAGE_SIZE);
      addr += MiB(2);
    }
  }

  set_cr3((uint64)pml4_table);
}
