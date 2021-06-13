#include "fat.h"
#include "serial.h"

void fat_init(Fat *fat, void *volume_image)
{
    fat->bpb = (BPB*)volume_image;
}

static uint64 cluster_address(Fat *fat, int cluster)
{
    uint64 sector_num = (fat->bpb->reserved_sector_count +
                         (fat->bpb->num_fats *
                          fat->bpb->fat_size_32) +
                         (cluster - 2) * fat->bpb->sectors_per_cluster);
    uint64 offset = sector_num * fat->bpb->bytes_per_sector;
    return (uint64)fat->bpb + offset;
}

void fat_list(Fat *fat)
{
    DirectoryEntry* dir_base = (DirectoryEntry*)cluster_address(fat, fat->bpb->root_cluster);
    int entries_per_cluster = (fat->bpb->bytes_per_sector / sizeof(DirectoryEntry) *
                               fat->bpb->sectors_per_cluster);
    for (int i = 0; i < entries_per_cluster; i++) {
        DirectoryEntry *file = dir_base + i;

        if (file->name[0] == 0x00) break;
        if (file->name[0] == 0xe5) continue;
        if (file->name[0] == 0x05) continue;
        if (file->attr & ATTR_HIDDEN) continue;

        for (int i = 0; i < 11; i++) {
            print_char(file->name[i]);
        }
        print_char('\n');
    }
}
