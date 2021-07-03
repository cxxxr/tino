#ifndef EFI_H_
#define EFI_H_

#include "../kernel/int.h"

/// guid
typedef struct {
  uint32 data1;
  uint16 data2;
  uint16 data3;
  uint8 data4[8];
} EFI_GUID;

#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID                                      \
  {                                                                            \
    0x9042A9DE, 0x23DC, 0x4A38, {                                              \
      0x96, 0xFB, 0x7A, 0xDE, 0xD0, 0x80, 0x51, 0x6A                           \
    }                                                                          \
  }

#define EFI_LOADED_IMAGE_PROTOCOL_GUID                                         \
  {                                                                            \
    0x5B1B31A1, 0x9562, 0x11d2, {                                              \
      0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B                           \
    }                                                                          \
  }

#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID                                   \
  {                                                                            \
    0x0964e5b22, 0x6459, 0x11d2, {                                             \
      0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b                           \
    }                                                                          \
  }

#define EFI_FILE_INFO_ID                                                       \
  {                                                                            \
    0x9576e92, 0x6d3f, 0x11d2, {                                               \
      0x8e, 0x39, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b                            \
    }                                                                          \
  }

///
#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL 0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL 0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL 0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER 0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE 0x00000020

///
typedef void *EFI_HANDLE;
typedef void *EFI_EVENT;
typedef uint64 EFI_STATUS;
typedef uint64 EFI_PHYISICAL_ADDRESS;
typedef uint64 EFI_VIRTUAL_ADDRESS;

/// table header
typedef struct {
  uint64 signature;
  uint32 revision;
  uint32 header_size;
  uint32 crc32;
  uint32 reserved;
} EFI_TABLE_HEADER;

/// memory type
typedef enum {
  EFI_RESERVED_MEMORY_TYPE,
  EFI_LOADER_CODE,
  EFI_LOADER_DATA,
  EFI_BOOT_SERVICES_CODE,
  EFI_BOOT_SERVICES_DATA,
  EFI_RUNTIME_SERVICES_CODE,
  EFI_RUNTIME_SERVICES_DATA,
  EFI_CONVENTIONAL_MEMORY,
  EFI_UNUSABLE_MEMORY,
  EFI_ACPI_RECLAIM_MEMORY,
  EFI_ACPI_MEMORY_NVS,
  EFI_MEMORY_MAPPED_IO,
  EFI_MEMORY_MAPPED_IO_PORT_SPACE,
  EFI_PAL_CODE,
  EFI_PERSISTENT_MEMORY,
  EFI_MAX_MEMORY_TYP,
} EFI_MEMORY_TYPE;

/// efi time
typedef struct {
  uint16 year;
  uint8 month;
  uint8 day;
  uint8 hour;
  uint8 minute;
  uint8 second;
  uint8 pad1;
  uint32 nanosecond;
  uint16 time_zone;
  uint8 daylight;
  uint8 pad2;
} EFI_TIME;

/// simple text input protocol
typedef struct {
  void *reset;
  void *read_key_stroke;
  EFI_EVENT wait_for_key;
} EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

/// simple text output protocol
struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef EFI_STATUS (*EFI_TEXT_STRING)(
    struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, char16 *String);

typedef EFI_STATUS (*EFI_TEXT_SET_CURSOR_POSITION)(
    struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, uintn column, uintn row);

typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
  void *reset;
  EFI_TEXT_STRING output_string;
  void *test_string;
  void *query_mode;
  void *set_mode;
  void *set_attribute;
  void *clear_screen;
  EFI_TEXT_SET_CURSOR_POSITION set_cursor_position;
  void *enable_cursor;
  void *mode;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

/// graphics output protocol
typedef struct {
  uint32 red_mask;
  uint32 green_mask;
  uint32 blue_mask;
  uint32 reserved_mask;
} EFI_PIXEL_BITMASK;

typedef enum {
  PixelRedGreenBlueReserved8BitPerColor,
  PixelBlueGreenRedReserved8BitPerColor,
  PixelBitMask,
  PixelBltOnly,
  PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION {
  uint32 version;
  uint32 horizontal_resolution;
  uint32 vertical_resolution;
  EFI_GRAPHICS_PIXEL_FORMAT pixel_format;
  EFI_PIXEL_BITMASK pixel_inforamtion;
  uint32 pixels_per_scan_line;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
  uint32 max_mode;
  uint32 mode;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
  uintn size_of_info;
  uintn frame_buffer_base;
  uintn frame_buffer_size;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef struct {
  void * /*EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE*/ query_mode;
  void * /*EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE*/ set_mode;
  void * /*EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT*/ blt;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *mode;
} EFI_GRAPHICS_OUTPUT_PROTOCOL;

/// file protocol
#define EFI_FILE_MODE_READ 0x0000000000000001

struct EFI_FILE_PROTOCOL;

typedef EFI_STATUS (*EFI_FILE_OPEN)(struct EFI_FILE_PROTOCOL *This,
                                    struct EFI_FILE_PROTOCOL **NewHandle,
                                    char16 *FileName, uint64 OpenMode,
                                    uint64 Attributes);

typedef EFI_STATUS (*EFI_FILE_GET_INFO)(struct EFI_FILE_PROTOCOL *This,
                                        EFI_GUID *InformationType,
                                        uintn *BufferSize, void *Buffer);

typedef EFI_STATUS (*EFI_FILE_READ)(struct EFI_FILE_PROTOCOL *this,
                                    uintn *buffer_size, void *buffer);

typedef struct EFI_FILE_PROTOCOL {
  uint64 revision;
  EFI_FILE_OPEN open;
  void *close;
  void *delete;
  EFI_FILE_READ read;
  void *write;
  void *get_position;
  void *set_position;
  EFI_FILE_GET_INFO get_info;
  void *set_info;
  void *flush;
  void *open_ex;
  void *read_ex;
  void *write_ex;
  void *flush_ex;
} EFI_FILE_PROTOCOL;

/// efi file info
typedef struct {
  uint64 size;
  uint64 file_size;
  uint64 physical_size;
  EFI_TIME create_time;
  EFI_TIME last_access_time;
  EFI_TIME modification_time;
  uint64 attribute;
  char16 file_name[];
} EFI_FILE_INFO;

/// simple file system protocol
struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

typedef EFI_STATUS (*EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME)(
    struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This, EFI_FILE_PROTOCOL **Root);

typedef struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
  uint64 revision;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME open_volume;
} EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

/// boot services
typedef struct {
  uint32 type;
  EFI_PHYISICAL_ADDRESS physical_start;
  EFI_VIRTUAL_ADDRESS virtual_start;
  uint64 number_of_pages;
  uint64 attribute;
} EFI_MEMORY_DESCRIPTOR;

typedef EFI_STATUS (*EFI_GET_MEMORY_MAP)(uintn *MemoryMapSize,
                                         EFI_MEMORY_DESCRIPTOR *MemoryMap,
                                         uintn *MapKey, uintn *DescriptorSize,
                                         uintn *DescriptorVersion);

typedef EFI_STATUS (*EFI_ALLOCATE_POOL)(EFI_MEMORY_TYPE pool_type, uintn size,
                                        void **buffer);

typedef EFI_STATUS (*EFI_EXIT_BOOT_SERVICES)(EFI_HANDLE, uintn);

typedef enum {
  AllHandles,
  ByRegisterNotify,
  ByProtocol
} EFI_LOCATE_SEARCH_TYPE;

typedef EFI_STATUS (*EFI_LOCATE_HANDLE_BUFFER)(
    EFI_LOCATE_SEARCH_TYPE SearchType, EFI_GUID *Protocol, void *SearchKey,
    uintn *NoHandles, EFI_HANDLE **Buffer);

typedef EFI_STATUS (*EFI_OPEN_PROTOCOL)(EFI_HANDLE Handle, EFI_GUID *Protocol,
                                        void **Interface,
                                        EFI_HANDLE AgentHandle,
                                        EFI_HANDLE ControllerHandle,
                                        uint32 Attributes);

typedef EFI_STATUS (*EFI_FREE_POOL)(void *Buffer);

typedef void (*EFI_COPY_MEM)(void *destination, void *source, uintn length);

typedef void (*EFI_SET_MEM)(void *buffer, uintn size, uint8 value);

typedef enum {
  ALLOCATE_ANY_PAGES,
  ALLOCATE_MAX_ADDRESS,
  ALLOCATE_ADDRESS,
  MAX_ALLOCATE_TYPE
} EFI_ALLOCATE_TYPE;

typedef uint64 EFI_PHYSICAL_ADDRESS;

typedef EFI_STATUS (*EFI_ALLOCATE_PAGES)(EFI_ALLOCATE_TYPE type,
                                         EFI_MEMORY_TYPE memory_type,
                                         uintn pages,
                                         EFI_PHYSICAL_ADDRESS *memory);

typedef struct EFI_BOOT_SERVICES {
  ///
  /// The table header for the EFI Boot Services Table.
  ///
  EFI_TABLE_HEADER hdr;

  //
  // Task Priority Services
  //
  void *raise_tpl;
  void *restore_tpl;

  //
  // Memory Services
  //
  EFI_ALLOCATE_PAGES allocate_pages;
  void *free_pages;
  EFI_GET_MEMORY_MAP get_memory_map;
  EFI_ALLOCATE_POOL allocate_pool;
  EFI_FREE_POOL free_pool;

  //
  // Event & Timer Services
  //
  void *create_event;
  void *set_timer;
  void *wait_for_event;
  void *signal_event;
  void *close_event;
  void *check_event;

  //
  // Protocol Handler Services
  //
  void *install_protocol_interface;
  void *reinstall_protocol_interface;
  void *uninstall_protocol_interface;
  void *handle_protocol;
  void *reserved;
  void *register_protocol_notify;
  void *locate_handle;
  void *locate_device_path;
  void *install_configuration_table;

  //
  // Image Services
  //
  void *load_image;
  void *start_image;
  void *exit;
  void *unload_image;
  EFI_EXIT_BOOT_SERVICES exit_boot_services;

  //
  // Miscellaneous Services
  //
  void *get_next_monotonic_count;
  void *stall;
  void *set_watchdog_timer;

  //
  // DriverSupport Services
  //
  void *connect_controller;
  void *disconnect_controller;

  //
  // Open and Close Protocol Services
  //
  EFI_OPEN_PROTOCOL open_protocol;
  void *close_protocol;
  void *open_protocol_information;

  //
  // Library Services
  //
  void *protocols_per_handle;
  EFI_LOCATE_HANDLE_BUFFER locate_handle_buffer;
  void *locate_protocol;
  void *install_multiple_protocol_interfaces;
  void *uninstall_multiple_protocol_interfaces;

  //
  // 32-bit CRC Services
  //
  void *calculate_crc32;

  //
  // Miscellaneous Services
  //
  EFI_COPY_MEM copy_mem;
  EFI_SET_MEM set_mem;
  void *create_event_ex;
} EFI_BOOT_SERVICES;

/// system table
typedef struct {
  EFI_TABLE_HEADER hdr;
  char16 *firmware_vendor;
  uint32 firmware_revision;
  EFI_HANDLE console_in_handle;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL *con_in;
  EFI_HANDLE ConsoleOutHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *con_out;
  EFI_HANDLE StandardErrorHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *std_err;
  void *runtime_services;
  EFI_BOOT_SERVICES *boot_services;
  uintn number_of_table_entries;
  void *configuration_table;
} EFI_SYSTEM_TABLE;

/// loaded image protocol
typedef struct {
  uint32 revision;
  EFI_HANDLE parent_handle;
  EFI_SYSTEM_TABLE *system_table;

  EFI_HANDLE device_handle;
  void /*EFI_DEVICE_PATH_PROTOCOL*/ *file_path;
  void *reserved;

  uint32 load_option_size;
  void *load_options;

  void *image_base;
  uint64 image_size;
  EFI_MEMORY_TYPE image_code_type;
  EFI_MEMORY_TYPE image_data_type;
  void * /*EFI_IMAGE_UNLOAD*/ un_load;
} EFI_LOADED_IMAGE_PROTOCOL;

#define EFI_SUCCESS 0

#endif
