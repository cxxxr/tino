#include "fat.h"
#include "serial.h"
#include "general.h"

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

static int entries_per_cluster(Fat *fat)
{
    return (fat->bpb->bytes_per_sector / sizeof(DirectoryEntry) *
            fat->bpb->sectors_per_cluster);
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

static FileName make_file_name(const unsigned char *name, const unsigned char *ext)
{
    FileName file;
    for (int i = 0; i < 9; i++) file.name[i] = 0;
    for (int i = 0; i < 4; i++) file.ext[i] = 0;

    for (int i = 0; name[i]; i++) {
        file.name[i] = name[i];
    }
    for (int i = 0; ext[i]; i++) {
        file.ext[i] = ext[i];
    }
    return file;
}

static void print_file_name(FileName filename)
{
    print_string(filename.name);
    print_char('.');
    print_string(filename.ext);
    print_char('\n');
}

void fat_list_root_dir(Fat * fat)
{
    DirectoryEntry *dir_base =
        (DirectoryEntry *) cluster_address(fat, fat->bpb->root_cluster);
    for (int i = 0; i < entries_per_cluster(fat); i++) {
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
        print_file_name(filename);
    }
}

static int is_same_file(FileName file1, FileName file2)
{
    for (int i = 0; i < 9; i++) {
        if (file1.name[i] != file2.name[i]) {
            // print_string("diff name: ");
            // print_char(file1.name[i]);
            // print_char(' ');
            // print_char(file2.name[i]);
            // print_char('\n');
            return 0;
        }
        if (file1.name[i] == 0) break;
    }

    for (int i = 0; i < 4; i++) {
        if (file1.ext[i] != file2.ext[i]) {
            // print_string("diff ext: ");
            // print_uint64(file1.ext[i]);
            // print_char(' ');
            // print_uint64(file2.ext[i]);
            // print_char('\n');
            return 0;
        }
        if (file1.ext[i] == 0) break;
    }

    return 1;
}

static unsigned long next_cluster(Fat *fat, unsigned long cluster)
{
    uint64 fat_offset = fat->bpb->reserved_sector_count * fat->bpb->bytes_per_sector;
    uint32 *allocation_table = (uint32*)((uint32)fat->bpb + fat_offset);
    return allocation_table[cluster];
}

static int is_eoc(uint32 cluster)
{
    return cluster >= 0x0ffffff8ul;
}

static DirectoryEntry *find_directory_entry(Fat *fat, FileName filename, uint32 dir_cluster)
{
    DirectoryEntry *dir_base = (DirectoryEntry *) cluster_address(fat, dir_cluster);

    while (!is_eoc(dir_cluster)) {
        for (int i = 0; i < entries_per_cluster(fat); i++) {
            DirectoryEntry *entry = dir_base + i;
            if (is_same_file(convert_file_name(entry), filename))
                return entry;
        }
        dir_cluster = next_cluster(fat, dir_cluster);
    }

    return NULL;
}

void fat_test(Fat *fat)
{
    FileName file = make_file_name("HELLO", "C");

    DirectoryEntry *entry = find_directory_entry(fat, file, fat->bpb->root_cluster);
    if (entry) {
        print_string("OK\n");
    } else {
        print_string("NG\n");
    }
}
