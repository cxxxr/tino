#include "efi.h"

#define NULL 0

void __chkstk(void) { return; }

void print_uint64(EFI_SYSTEM_TABLE *SystemTable, uint64 value) {
  CHAR16 buffer[21];
  int i = 20, column = 0;
  for (;;) {
    buffer[--i] = (value % 10) + '0';
    column++;
    value /= 10;
    if (value == 0)
      break;
  }
  buffer[20] = '\0';
  SystemTable->con_out->output_string(SystemTable->con_out, buffer + i);
  SystemTable->con_out->output_string(SystemTable->con_out, L" ");
}

EFI_STATUS open_gop(EFI_SYSTEM_TABLE *SystemTable, EFI_HANDLE ImageHandle,
                    EFI_GRAPHICS_OUTPUT_PROTOCOL **gop) {
  uintn num_gop_handles = 0;
  EFI_HANDLE *gop_handles = NULL;
  EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  EFI_STATUS status;
  status = SystemTable->boot_services->locate_handle_buffer(
      ByProtocol, &gop_guid, NULL, &num_gop_handles, &gop_handles);
  if (status != EFI_SUCCESS)
    return status;

  status = SystemTable->boot_services->open_protocol(
      gop_handles[0], &gop_guid, (void **)&gop, ImageHandle, NULL,
      EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
  if (status != EFI_SUCCESS)
    return status;

  status = SystemTable->boot_services->free_pool(gop_handles);

  return status;
}

EFI_STATUS open_root_dir(EFI_SYSTEM_TABLE *SystemTable,
                         EFI_HANDLE ImageHandle,
                         EFI_FILE_PROTOCOL **root_dir) {
  EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;

  EFI_GUID loaded_image_protocol_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
  EFI_GUID simple_file_system_protocol_guid =
      EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

  EFI_STATUS status;
  status = SystemTable->boot_services->open_protocol(
      ImageHandle, &loaded_image_protocol_guid, (void **)&loaded_image,
      ImageHandle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
  if (status != EFI_SUCCESS)
    return status;

  status = SystemTable->boot_services->open_protocol(
      loaded_image->device_handle, &simple_file_system_protocol_guid,
      (void **)&fs, ImageHandle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
  if (status != EFI_SUCCESS)
    return status;

  return fs->open_volume(fs, root_dir);
}

EFI_STATUS EfiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
  uint8 memory_map_buffer[1024 * 16];
  uintn memory_map_size = sizeof(memory_map_buffer);
  EFI_MEMORY_DESCRIPTOR *memory_map =
      (EFI_MEMORY_DESCRIPTOR *)memory_map_buffer;
  uintn map_key;
  uintn descriptor_size;
  uintn descriptor_version;

  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
  if (open_gop(SystemTable, ImageHandle, &gop) != EFI_SUCCESS) {
    SystemTable->con_out->output_string(SystemTable->con_out, L"ERROR!");
  }

  EFI_FILE_PROTOCOL *root_dir;
  if (open_root_dir(SystemTable, ImageHandle, &root_dir) != EFI_SUCCESS) {
    SystemTable->con_out->output_string(SystemTable->con_out, L"ERROR!");
  }

  SystemTable->boot_services->get_memory_map(&memory_map_size, memory_map,
                                          &map_key, &descriptor_size,
                                          &descriptor_version);

  if (SystemTable->boot_services->exit_boot_services(ImageHandle, map_key) !=
      EFI_SUCCESS) {
    SystemTable->con_out->output_string(SystemTable->con_out, L"ERROR!");
  }

  // SystemTable->con_out->output_string(SystemTable->con_out, L"Hello, world!\n");
  // SystemTable->con_out->SetCursorPosition(SystemTable->con_out, 0, 1);
  // SystemTable->con_out->output_string(SystemTable->con_out, L"Hello, world!\n");
  // SystemTable->con_out->SetCursorPosition(SystemTable->con_out, 0, 2);
  // SystemTable->con_out->output_string(SystemTable->con_out, L"Hello, world!\n");

  // int row = 1;
  // for (uintn iter = (uintn)memory_map;
  //      iter < (uintn)(memory_map + memory_map_size);
  //      iter += descriptor_size) {
  //   EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)iter;
  //   SystemTable->con_out->SetCursorPosition(SystemTable->con_out, 0, row);
  //   print_uint64(SystemTable, desc->type);
  //   print_uint64(SystemTable, desc->physical_start);
  //   print_uint64(SystemTable, desc->virtual_start);
  //   print_uint64(SystemTable, desc->number_of_pages);
  //   print_uint64(SystemTable, desc->attribute);
  //   if (row == 26) break;
  //   row++;
  // }

  while (1)
    ;
  return 0;
}
