#include "fat.h"
#include "primitive.h"
#include "serial.h"

static FileName convert_file_name(DirectoryEntry *entry);
static FileName make_file_name(const char *name, const char *ext);
static int is_same_file(FileName file1, FileName file2);
static void print_file_name(FileName filename);

void fat_init(Fat *fat, void *volume_image) {
  fat->bpb = (BPB *)volume_image;
  fat->bytes_per_cluster =
      fat->bpb->bytes_per_sector * fat->bpb->sectors_per_cluster;
  fat->entries_per_cluster =
      (fat->bpb->bytes_per_sector / sizeof(DirectoryEntry) *
       fat->bpb->sectors_per_cluster);
}

static uint64 cluster_address(Fat *fat, int cluster) {
  uint64 sector_num = (fat->bpb->reserved_sector_count +
                       (fat->bpb->num_fats * fat->bpb->fat_size_32) +
                       (cluster - 2) * fat->bpb->sectors_per_cluster);
  uint64 offset = sector_num * fat->bpb->bytes_per_sector;
  return (uint64)fat->bpb + offset;
}

void fat_list_root_dir(Fat *fat) {
  DirectoryEntry *dir_base =
      (DirectoryEntry *)cluster_address(fat, fat->bpb->root_cluster);
  for (int i = 0; i < fat->entries_per_cluster; i++) {
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

static unsigned long next_cluster(Fat *fat, unsigned long cluster) {
  uint64 fat_offset =
      fat->bpb->reserved_sector_count * fat->bpb->bytes_per_sector;
  uint32 *allocation_table = (uint32 *)((uint32)fat->bpb + fat_offset);
  return allocation_table[cluster];
}

static int is_eoc(uint32 cluster) { return cluster >= 0x0ffffff8ul; }

static DirectoryEntry *find_directory_entry(Fat *fat, FileName filename,
                                            uint32 dir_cluster) {
  while (!is_eoc(dir_cluster)) {
    DirectoryEntry *dir_base =
        (DirectoryEntry *)cluster_address(fat, dir_cluster);
    for (int i = 0; i < fat->entries_per_cluster; i++) {
      DirectoryEntry *entry = dir_base + i;
      if (is_same_file(convert_file_name(entry), filename))
        return entry;
    }
    dir_cluster = next_cluster(fat, dir_cluster);
  }

  return NULL;
}

static uint32 directory_entry_first_cluster(DirectoryEntry *entry) {
  return (entry->first_cluster_low | ((uint32)entry->first_cluster_high << 16));
}

void fat_open_file(Fat *fat, FileName filename) {
  DirectoryEntry *entry =
      find_directory_entry(fat, filename, fat->bpb->root_cluster);

  if (!entry)
    return;

  uint32 remain_bytes = entry->file_size;

  uint32 cluster = directory_entry_first_cluster(entry);
  while (!is_eoc(cluster)) {
    char *p = (char *)cluster_address(fat, cluster);
    for (int i = 0; i < fat->bytes_per_cluster && i < remain_bytes; i++) {
      print_char(*p);
      p++;
    }
    remain_bytes -= fat->bytes_per_cluster;
    cluster = next_cluster(fat, cluster);
  }
}

void fat_test(Fat *fat) {
  FileName filename = make_file_name("HELLO", "C");

  fat_open_file(fat, filename);
}

static FileName convert_file_name(DirectoryEntry *entry) {
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

  for (int i = 2; i >= 0; i--) {
    if (entry->name[8 + i] == ' ') {
      filename.ext[i] = 0;
    } else {
      break;
    }
  }

  return filename;
}

static FileName make_file_name(const char *name, const char *ext) {
  FileName filename;
  for (int i = 0; i < 9; i++)
    filename.name[i] = 0;
  for (int i = 0; i < 4; i++)
    filename.ext[i] = 0;

  for (int i = 0; name[i]; i++) {
    filename.name[i] = name[i];
  }
  for (int i = 0; ext[i]; i++) {
    filename.ext[i] = ext[i];
  }
  return filename;
}

static int is_same_file(FileName filename1, FileName filename2) {
  for (int i = 0; i < 9; i++) {
    if (filename1.name[i] != filename2.name[i]) {
      return 0;
    }
    if (filename1.name[i] == 0)
      break;
  }

  for (int i = 0; i < 4; i++) {
    if (filename1.ext[i] != filename2.ext[i]) {
      return 0;
    }
    if (filename1.ext[i] == 0)
      break;
  }

  return 1;
}

static void print_file_name(FileName filename) {
  print_string(filename.name);
  if (filename.ext[0]) {
    print_char('.');
    print_string(filename.ext);
  }
  print_char('\n');
}
