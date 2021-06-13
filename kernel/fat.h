#ifndef FAT_H_
#define FAT_H_

#include "int.h"

struct BPB {
  uint8 jump_boot[3];
  char oem_name[8];
  uint16 bytes_per_sector;
  uint8 sectors_per_cluster;
  uint16 reserved_sector_count;
  uint8 num_fats;
  uint16 root_entry_count;
  uint16 total_sectors_16;
  uint8 media;
  uint16 fat_size_16;
  uint16 sectors_per_track;
  uint16 num_heads;
  uint32 hidden_sectors;
  uint32 total_sectors_32;
  uint32 fat_size_32;
  uint16 ext_flags;
  uint16 fs_version;
  uint32 root_cluster;
  uint16 fs_info;
  uint16 backup_boot_sector;
  uint8 reserved[12];
  uint8 drive_number;
  uint8 reserved1;
  uint8 boot_signature;
  uint32 volume_id;
  char volume_label[11];
  char fs_type[8];
} __attribute__((packed));

typedef struct BPB BPB;

typedef struct {
    BPB *bpb;
} Fat;

#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN    0x02
#define ATTR_SYSTEM    0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE   0x20
#define ATTR_LONG_NAME (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)

struct DirectoryEntry {
    unsigned char name[11];
    uint8 attr;
    uint8 ntres;
    uint8 create_time_tenth;
    uint16 create_time;
    uint16 create_date;
    uint16 last_access_date;
    uint16 first_cluster_high;
    uint16 write_time;
    uint16 write_date;
    uint16 first_cluster_low;
    uint32 file_size;
} __attribute__((packed));

typedef struct DirectoryEntry DirectoryEntry;

typedef struct {
  char name[9];
  char ext[4];
} FileName;

void fat_init(Fat *fat, void *volume_image);
void fat_list_root_dir(Fat *fat);

#endif
