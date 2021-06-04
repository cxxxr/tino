#include "gdt.h"
#include "asmfunc.h"

// https://wiki.osdev.org/Global_Descriptor_Table

uint64 make_gdt_descriptor(uint8 rw_flag, uint8 ex_flag, uint8 long_mode)
{
    return ((uint64)rw_flag << 41       | // Access Byte RW
            (uint64)ex_flag << 43       | // Access Byte Ex
            (uint64)1 << 44             | // Access Byte S
            (uint64)0 << 45             | // Privl
            (uint64)1 << 47             | // Pr
            (uint64)long_mode << 53     | // Long mode, wikiでは0になってる？
            0                           | // Sz
            (uint64)1 << 55             | // Gr, 1にすると4KiB単位
            0);
}

uint64 make_gdt_kernel_code(void)
{
    return make_gdt_descriptor(1, 1, 1);
}

uint64 make_gdt_kernel_data(void)
{
    return make_gdt_descriptor(1, 0, 0);
}

struct gdtr {
    uint16 limit;
    void *base;
} __attribute__((__packed__));

static struct gdtr gdtr;

void init_gdt(void)
{
    gdt[0] = 0;
    gdt[1] = make_gdt_kernel_code();
    gdt[2] = make_gdt_kernel_data();

    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base = &gdt;
    load_gdtr(&gdtr);
}
