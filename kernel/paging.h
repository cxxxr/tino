#ifndef PAGING_H_
#define PAGING_H_

#include "int.h"

typedef union {
  uint64 value;
  struct {
    uint64 offset : 12;
    uint64 page : 9;
    uint64 dir : 9;
    uint64 pdp : 9;
    uint64 pml4 : 9;
    uint64 : 16;
  } __attribute__((packed)) parts;
} VirtualAddress;

typedef union {
  uint64 value;
  struct {
    uint8 present : 1;
    uint8 rw : 1;
    uint8 us : 1;
    uint8 pwt : 1;
    uint8 pcd : 1;
    uint8 accessed : 1;
    uint8 : 6;
    uint64 paddr : 40;
    uint64 : 12;
  } bits;
} __attribute__((packed)) PageMapEntry;

void init_page_table(void);

#endif
