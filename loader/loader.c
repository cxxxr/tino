#include "../kernel/elf.h"
#include "../kernel/main.h"
#include "efi.h"

#define NULL 0

#define ASSERT(status)                                                         \
  {                                                                            \
    EFI_STATUS status0 = status;                                               \
    if (status0 != EFI_SUCCESS) {                                              \
      while (1)                                                                \
        __asm__("hlt");                                                        \
    }                                                                          \
  }

const EFI_PHYSICAL_ADDRESS kernel_base_addr = 0x100000;

static EntryParams entry_params;

typedef void EntryPointType(EntryParams *);

void __chkstk(void) { return; }

void print_string(EFI_SYSTEM_TABLE *system_table, char16 *string) {
  system_table->con_out->output_string(system_table->con_out, string);
}

void print_uint64(EFI_SYSTEM_TABLE *SystemTable, uint64 value) {
#define BUFFER_SIZE 100
  char16 buffer[BUFFER_SIZE];
  int i = BUFFER_SIZE, column = 0;
#undef BUFFER_SIZE
  buffer[--i] = '\0';
  buffer[--i] = '\n';
  buffer[--i] = '\r';
  for (;;) {
    buffer[--i] = (value % 10) + '0';
    column++;
    value /= 10;
    if (value == 0) {
      break;
    }
  }
  SystemTable->con_out->output_string(SystemTable->con_out, buffer + i);
}

void print_uint64_hex(EFI_SYSTEM_TABLE *SystemTable, uint64 value) {
#define BUFFER_SIZE 100
  char16 buffer[BUFFER_SIZE];
  int i = BUFFER_SIZE, column = 0;
#undef BUFFER_SIZE
  buffer[--i] = '\0';
  buffer[--i] = '\n';
  buffer[--i] = '\r';
  for (;;) {
    buffer[--i] = "0123456789abcdef"[(value % 16)];
    column++;
    value /= 16;
    if (value == 0) {
      break;
    }
  }
  SystemTable->con_out->output_string(SystemTable->con_out, buffer + i);
}

void open_gop(EFI_SYSTEM_TABLE *system_table, EFI_HANDLE image_handle,
              EFI_GRAPHICS_OUTPUT_PROTOCOL **gop) {
  uintn num_gop_handles = 0;
  EFI_HANDLE *gop_handles = NULL;
  EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

  ASSERT(system_table->boot_services->locate_handle_buffer(
      ByProtocol, &gop_guid, NULL, &num_gop_handles, &gop_handles));

  ASSERT(system_table->boot_services->open_protocol(
      gop_handles[0], &gop_guid, (void **)gop, image_handle, NULL,
      EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL));

  ASSERT(system_table->boot_services->free_pool(gop_handles));
}

void open_root_dir(EFI_SYSTEM_TABLE *system_table, EFI_HANDLE image_handle,
                   EFI_FILE_PROTOCOL **root_dir) {
  EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;

  EFI_GUID loaded_image_protocol_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
  EFI_GUID simple_file_system_protocol_guid =
      EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

  ASSERT(system_table->boot_services->open_protocol(
      image_handle, &loaded_image_protocol_guid, (void **)&loaded_image,
      image_handle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL));

  ASSERT(system_table->boot_services->open_protocol(
      loaded_image->device_handle, &simple_file_system_protocol_guid,
      (void **)&fs, image_handle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL));

  ASSERT(fs->open_volume(fs, root_dir));
}

void read_file(EFI_SYSTEM_TABLE *system_table, EFI_FILE_PROTOCOL *file,
               void **buffer) {
  EFI_GUID file_info_guid = EFI_FILE_INFO_ID;

  uintn file_info_size = sizeof(EFI_FILE_INFO) + sizeof(char16) * 12;
  uint8 file_info_buffer[file_info_size];
  file->get_info(file, &file_info_guid, &file_info_size, file_info_buffer);
  EFI_FILE_INFO *file_info = (EFI_FILE_INFO *)file_info_buffer;
  uintn file_size = file_info->file_size;
  ASSERT(system_table->boot_services->allocate_pool(EFI_LOADER_DATA, file_size,
                                                    buffer));
  ASSERT(file->read(file, &file_size, *buffer));
}

void get_load_address_range(Elf64_Ehdr *ehdr, uintn *first_addr,
                            uintn *last_addr) {
  Elf64_Phdr *phdr = (Elf64_Phdr *)((uint64)ehdr + ehdr->e_phoff);
  uintn first = 0xFFFFFFFFFFFFFFFFULL;
  uintn last = 0;

  for (Elf64_Half i = 0; i < ehdr->e_phnum; i++) {
    if (phdr[i].p_type == PT_LOAD) {
      if (first > phdr[i].p_vaddr)
        first = phdr[i].p_vaddr;
      if (last < phdr[i].p_vaddr + phdr[i].p_memsz)
        last = phdr[i].p_vaddr + phdr[i].p_memsz;
    }
  }

  *first_addr = first;
  *last_addr = last;
}

void copy_load_segments(EFI_SYSTEM_TABLE *system_table, Elf64_Ehdr *ehdr) {
  Elf64_Phdr *phdr = (Elf64_Phdr *)((uint64)ehdr + ehdr->e_phoff);

  print_string(system_table, L"loading kernel.elf");

  for (Elf64_Half i = 0; i < ehdr->e_phnum; i++) {
    print_string(system_table, L"\r\ni: ");
    print_uint64(system_table, i);
    print_string(system_table, L"type: ");
    print_uint64_hex(system_table, phdr[i].p_type);
    print_string(system_table, L"flags: ");
    print_uint64_hex(system_table, phdr[i].p_flags);
    print_string(system_table, L"offset: ");
    print_uint64_hex(system_table, phdr[i].p_offset);
    print_string(system_table, L"vaddr: ");
    print_uint64_hex(system_table, phdr[i].p_vaddr);
    print_string(system_table, L"paddr: ");
    print_uint64_hex(system_table, phdr[i].p_paddr);
    print_string(system_table, L"filesz: ");
    print_uint64_hex(system_table, phdr[i].p_filesz);
    print_string(system_table, L"memsz: ");
    print_uint64_hex(system_table, phdr[i].p_memsz);
    print_string(system_table, L"align: ");
    print_uint64_hex(system_table, phdr[i].p_align);

    if (phdr[i].p_type == PT_LOAD) {
      uint64 source = (uint64)ehdr + phdr[i].p_offset;
      system_table->boot_services->copy_mem((void *)phdr[i].p_vaddr,
                                            (void *)source, phdr[i].p_filesz);
      uint64 remain_bytes = phdr[i].p_memsz - phdr[i].p_filesz;
      system_table->boot_services->set_mem(
          (void *)(phdr[i].p_vaddr + phdr[i].p_filesz), remain_bytes, 0);
    }
  }
}

void load_kernel(EFI_SYSTEM_TABLE *system_table, EFI_HANDLE image_handle,
                 EFI_FILE_PROTOCOL *root_dir) {
  EFI_FILE_PROTOCOL *kernel_file;
  ASSERT(root_dir->open(root_dir, &kernel_file, L"\\kernel.elf",
                        EFI_FILE_MODE_READ, 0));

  void *kernel_buffer;
  read_file(system_table, kernel_file, &kernel_buffer);

  Elf64_Ehdr *kernel_ehdr = (Elf64_Ehdr *)kernel_buffer;

  uintn kernel_first_addr, kernel_last_addr;
  get_load_address_range(kernel_ehdr, &kernel_first_addr, &kernel_last_addr);

  ASSERT(system_table->boot_services->allocate_pages(
      ALLOCATE_ADDRESS, EFI_LOADER_DATA,
      (kernel_last_addr - kernel_first_addr + 0xfff) / 0x1000,
      &kernel_first_addr));

  ASSERT(kernel_first_addr != kernel_base_addr);

  copy_load_segments(system_table, kernel_ehdr);

  ASSERT(system_table->boot_services->free_pool(kernel_buffer));
}

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) {
  uint8 memory_map_buffer[1024 * 16];
  uintn memory_map_size = sizeof(memory_map_buffer);
  EFI_MEMORY_DESCRIPTOR *memory_map =
      (EFI_MEMORY_DESCRIPTOR *)memory_map_buffer;
  uintn map_key;
  uintn descriptor_size;
  uintn descriptor_version;

  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
  open_gop(system_table, image_handle, &gop);

  EFI_FILE_PROTOCOL *root_dir;
  open_root_dir(system_table, image_handle, &root_dir);

  void *volume_image;

  {
    EFI_FILE_PROTOCOL *volume_file;
    root_dir->open(root_dir, &volume_file, L"\\fat_disk.img",
                   EFI_FILE_MODE_READ, 0);
    read_file(system_table, volume_file, &volume_image);
  }

  load_kernel(system_table, image_handle, root_dir);

  PixelFormat pixel_format;

  switch (gop->mode->info->pixel_format) {
  default:
  case PixelRedGreenBlueReserved8BitPerColor:
    pixel_format = PIXEL_RED_GREEN_BLUE_RESERVED_8BIT_PER_COLOR;
    break;
  case PixelBlueGreenRedReserved8BitPerColor:
    pixel_format = PIXEL_BLUE_GREEN_RED_RESERVED_8BIT_PER_COLOR;
    break;
  }

  FrameBuffer frame_buffer =
      (FrameBuffer){pixel_format,
                    gop->mode->info->pixels_per_scan_line,
                    gop->mode->info->horizontal_resolution,
                    gop->mode->info->vertical_resolution,
                    (uint8 *)gop->mode->frame_buffer_base,
                    gop->mode->frame_buffer_size};

  // print_uint64(system_table, (uint64)&entry_params);

  uint64 entry_addr = (uint64)((Elf64_Ehdr *)kernel_base_addr)->e_entry;

  {
    system_table->boot_services->get_memory_map(&memory_map_size, memory_map,
                                                &map_key, &descriptor_size,
                                                &descriptor_version);

    ASSERT(
        system_table->boot_services->exit_boot_services(image_handle, map_key));
  }

  entry_params.frame_buffer = &frame_buffer;
  entry_params.memory_map.base = memory_map;
  entry_params.memory_map.size = memory_map_size;
  entry_params.memory_map.descriptor_size = descriptor_size;
  entry_params.volume_image = volume_image;

  EntryParams *arg = &entry_params;
  ((EntryPointType *)(entry_addr))(arg);

  return 0;
}
