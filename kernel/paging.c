#include "paging.h"
#include "int.h"
#include "asmfunc.h"

#define SIZE 512
#define PDP_N 64

#define KiB(n) ((n) * 1024)
#define MiB(n) (KiB(n) * 1024)
#define GiB(n) (MiB(n) * 1024)

_Alignas(KiB(4)) static uint64 pml4_table[SIZE];
_Alignas(KiB(4)) static uint64 pdp_table[SIZE];
_Alignas(KiB(4)) static uint64 page_directory[PDP_N][SIZE];

// https://wiki.osdev.org/Paging

#define PAGE_DIRECTORY_RW (1 << 1)
#define PAGE_DIRECTORY_PRESENT (1 << 0)
#define PAGE_DIRECTORY_PAGE_SIZE (1 << 7) // 1なら2MiB, 0なら4KiB単位で扱うようになる

void init_page_table(void)
{
    const int flags = (PAGE_DIRECTORY_RW | PAGE_DIRECTORY_PRESENT);

    pml4_table[0] = (uint64)pdp_table | flags;

    for (int i = 1; i < SIZE; i++) {
        pml4_table[i] = 0;
    }

    for (int pdp_i = 0; pdp_i < PDP_N; pdp_i++) {
        pdp_table[pdp_i] = (uint64)(page_directory + pdp_i) | flags;
        for (int pd_i = 0; pd_i < SIZE; pd_i++) {
            page_directory[pdp_i][pd_i] = ((pd_i * MiB(2) + pdp_i * GiB(1)) |
                                           (flags | PAGE_DIRECTORY_PAGE_SIZE));
        }
    }

    set_cr3((uint64)pml4_table);
}
