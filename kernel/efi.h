#ifndef EFI_H_
#define EFI_H_

#include "int.h"

// typedef struct {
//     EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE QueryMode;
//     EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE SetMode;
//     EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT Blt;
//     EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE* Mode;
// } _EFI_GRAPHICS_OUTPUT_PROTOCOL;

typedef struct {
  uint32 RedMask;
  uint32 GreenMask;
  uint32 BlueMask;
  uint32 ReservedMask;
} _EFI_PIXEL_BITMASK;

typedef enum {
  _PixelRedGreenBlueReserved8BitPerColor,
  _PixelBlueGreenRedReserved8BitPerColor,
  _PixelBitMask,
  _PixelBltOnly,
  _PixelFormatMax
} _EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct _EFI_GRAPHICS_OUTPUT_MODE_INFORMATION {
  uint32 version;
  uint32 horizontal_resolution;
  uint32 vertical_resolution;
  _EFI_GRAPHICS_PIXEL_FORMAT pixel_format;
  _EFI_PIXEL_BITMASK pixel_inforamtion;
  uint32 pixels_per_scan_line;
} _EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
  uint32 max_mode;
  uint32 mode;
  _EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
  uintn frame_buffer_base;
  uintn frame_buffer_size;
} _EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef uint64 _EFI_PHYISICAL_ADDRESS;
typedef uint64 _EFI_VIRTUAL_ADDRESS;

typedef struct {
  uint32 type;
  _EFI_PHYISICAL_ADDRESS physical_start;
  _EFI_VIRTUAL_ADDRESS virtual_start;
  uint64 number_of_pages;
  uint64 attribute;
} _EFI_MEMORY_DESCRIPTOR;

typedef enum {
  _EFI_RESERVED_MEMORY_TYPE,
  _EFI_LOADER_CODE,
  _EFI_LOADER_DATA,
  _EFI_BOOT_SERVICES_CODE,
  _EFI_BOOT_SERVICES_DATA,
  _EFI_RUNTIME_SERVICES_CODE,
  _EFI_RUNTIME_SERVICES_DATA,
  _EFI_CONVENTIONAL_MEMORY,
  _EFI_UNUSABLE_MEMORY,
  _EFI_ACPI_RECLAIM_MEMORY,
  _EFI_ACPI_MEMORY_NVS,
  _EFI_MEMORY_MAPPED_IO,
  _EFI_MEMORY_MAPPED_IO_PORT_SPACE,
  _EFI_PAL_CODE,
  _EFI_PERSISTENT_MEMORY,
  _EFI_MAX_MEMORY_TYP,
} _EFI_MEMORY_TYPE;

#endif
