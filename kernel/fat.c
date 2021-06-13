#include "fat.h"
#include "serial.h"

void fat_init(Fat * fat, void *volume_image)
{
    fat->bpb = (BPB *) volume_image;
}

static uint64 cluster_address(Fat * fat, int cluster)
{
    uint64 sector_num = (fat->bpb->reserved_sector_count +
                         (fat->bpb->num_fats *
                          fat->bpb->fat_size_32) +
                         (cluster - 2) * fat->bpb->sectors_per_cluster);
    uint64 offset = sector_num * fat->bpb->bytes_per_sector;
    return (uint64) fat->bpb + offset;
}

static FileName convert_file_name(DirectoryEntry * entry)
{
    FileName filename;

    for (int i = 0; i < 8; i++)
        filename.name[i] = entry->name[i];
    for (int i = 0; i < 3; i++)
        filename.ext[i] = entry->name[8 + i];

    for (int i = 7; i >= 0; i--) {
        if (entry->name[i] == ' ') {
            filename.name[i] = 0;
        } else {
            break;
        }
    }

    for (int i = 3; i >= 0; i--) {
        if (entry->name[8 + i] == ' ') {
            filename.ext[i] = 0;
        } else {
            break;
        }
    }

    return filename;
}

void fat_list_root_dir(Fat * fat)
{
    DirectoryEntry *dir_base =
        (DirectoryEntry *) cluster_address(fat, fat->bpb->root_cluster);
    int entries_per_cluster =
        (fat->bpb->bytes_per_sector / sizeof(DirectoryEntry) *
         fat->bpb->sectors_per_cluster);
    for (int i = 0; i < entries_per_cluster; i++) {
        DirectoryEntry *entry = dir_base + i;

        if (entry->name[0] == 0x00)
            break;
        if (entry->name[0] == 0xe5)
            continue;
        if (entry->name[0] == 0x05)
            continue;
        if (entry->attr & ATTR_HIDDEN)
            continue;

        FileName filename = convert_file_name(entry);
        print_string(filename.name);
        print_char('.');
        print_string(filename.ext);
        print_char('\n');
    }
}
