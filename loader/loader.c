#include "efi.h"

#define NULL 0

void __chkstk(void) { return; }

EFI_STATUS open_gop(EFI_SYSTEM_TABLE *system_table, EFI_HANDLE image_handle,
                    EFI_GRAPHICS_OUTPUT_PROTOCOL **gop) {
  uintn num_gop_handles = 0;
  EFI_HANDLE *gop_handles = NULL;
  EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  EFI_STATUS status;
  status = system_table->boot_services->locate_handle_buffer(
      ByProtocol, &gop_guid, NULL, &num_gop_handles, &gop_handles);
  if (status != EFI_SUCCESS)
    return status;

  status = system_table->boot_services->open_protocol(
      gop_handles[0], &gop_guid, (void **)&gop, image_handle, NULL,
      EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
  if (status != EFI_SUCCESS)
    return status;

  status = system_table->boot_services->free_pool(gop_handles);

  return status;
}

EFI_STATUS open_root_dir(EFI_SYSTEM_TABLE *system_table,
                         EFI_HANDLE image_handle,
                         EFI_FILE_PROTOCOL **root_dir) {
  EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;

  EFI_GUID loaded_image_protocol_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
  EFI_GUID simple_file_system_protocol_guid =
      EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

  EFI_STATUS status;
  status = system_table->boot_services->open_protocol(
      image_handle, &loaded_image_protocol_guid, (void **)&loaded_image,
      image_handle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
  if (status != EFI_SUCCESS)
    return status;

  status = system_table->boot_services->open_protocol(
      loaded_image->device_handle, &simple_file_system_protocol_guid,
      (void **)&fs, image_handle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
  if (status != EFI_SUCCESS)
    return status;

  return fs->open_volume(fs, root_dir);
}

EFI_STATUS EfiMain(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) {
  uint8 memory_map_buffer[1024 * 16];
  uintn memory_map_size = sizeof(memory_map_buffer);
  EFI_MEMORY_DESCRIPTOR *memory_map =
      (EFI_MEMORY_DESCRIPTOR *)memory_map_buffer;
  uintn map_key;
  uintn descriptor_size;
  uintn descriptor_version;

  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
  if (open_gop(system_table, image_handle, &gop) != EFI_SUCCESS) {
    system_table->con_out->output_string(system_table->con_out, L"ERROR!");
  }

  EFI_FILE_PROTOCOL *root_dir;
  if (open_root_dir(system_table, image_handle, &root_dir) != EFI_SUCCESS) {
    system_table->con_out->output_string(system_table->con_out, L"ERROR!");
  }

  system_table->boot_services->get_memory_map(&memory_map_size, memory_map,
                                          &map_key, &descriptor_size,
                                          &descriptor_version);

  if (system_table->boot_services->exit_boot_services(image_handle, map_key) !=
      EFI_SUCCESS) {
    system_table->con_out->output_string(system_table->con_out, L"ERROR!");
  }

  while (1)
    ;
  return 0;
}
