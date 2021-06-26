#ifndef EFI_H_
#define EFI_H_

#include "../kernel/int.h"

typedef uint64 EFI_STATUS;
typedef unsigned short CHAR16;

typedef struct {
  uint32 Data1;
  uint16 Data2;
  uint16 Data3;
  uint8 Data4[8];
} GUID;

typedef GUID EFI_GUID;

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

typedef struct {
  uint64 Signature;
  uint32 Revision;
  uint32 HeaderSize;
  uint32 CRC32;
  uint32 Reserved;
} EFI_TABLE_HEADER;

typedef void *EFI_HANDLE;
typedef void *EFI_EVENT;

typedef struct {
  void *Reset;
  void *ReadKeyStroke;
  EFI_EVENT WaitForKey;
} EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef EFI_STATUS (*EFI_TEXT_STRING)(
    struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, CHAR16 *String);

typedef EFI_STATUS (*EFI_TEXT_SET_CURSOR_POSITION)(
    struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, uintn column, uintn row);

typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
  void *Reset;
  EFI_TEXT_STRING OutputString;
  void *TestString;
  void *QueryMode;
  void *SetMode;
  void *SetAttribute;
  void *ClearScreen;
  EFI_TEXT_SET_CURSOR_POSITION SetCursorPosition;
  void *EnableCursor;
  void *Mode;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

struct EFI_BOOT_SERVICES;

typedef struct {
  EFI_TABLE_HEADER Hdr;
  char16 *FirmwareVendor;
  uint32 FirmwareRevision;
  EFI_HANDLE ConsoleInHandle;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;
  EFI_HANDLE ConsoleOutHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
  EFI_HANDLE StandardErrorHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdErr;
  void *RuntimeServices;
  struct EFI_BOOT_SERVICES *BootServices;
  uintn NumberOfTableEntries;
  void *ConfigurationTable;
} EFI_SYSTEM_TABLE;

typedef struct {
  uint32 RedMask;
  uint32 GreenMask;
  uint32 BlueMask;
  uint32 ReservedMask;
} EFI_PIXEL_BITMASK;

typedef enum {
  _PixelRedGreenBlueReserved8BitPerColor,
  _PixelBlueGreenRedReserved8BitPerColor,
  _PixelBitMask,
  _PixelBltOnly,
  _PixelFormatMax
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
  uintn frame_buffer_base;
  uintn frame_buffer_size;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef struct {
  void * /*EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE*/ QueryMode;
  void * /*EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE*/ SetMode;
  void * /*EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT*/ Blt;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;
} EFI_GRAPHICS_OUTPUT_PROTOCOL;

typedef uint64 EFI_PHYISICAL_ADDRESS;
typedef uint64 EFI_VIRTUAL_ADDRESS;

typedef struct {
  uint32 type;
  EFI_PHYISICAL_ADDRESS physical_start;
  EFI_VIRTUAL_ADDRESS virtual_start;
  uint64 number_of_pages;
  uint64 attribute;
} EFI_MEMORY_DESCRIPTOR;

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

struct EFI_FILE_PROTOCOL;

typedef EFI_STATUS (*EFI_FILE_OPEN)(struct EFI_FILE_PROTOCOL *This,
                                    struct EFI_FILE_PROTOCOL **NewHandle,
                                    char16 *FileName, uint64 OpenMode,
                                    uint64 Attributes);

typedef EFI_STATUS (*EFI_FILE_GET_INFO)(struct EFI_FILE_PROTOCOL *This,
                                        EFI_GUID *InformationType,
                                        uintn *BufferSize, void *Buffer);

typedef struct EFI_FILE_PROTOCOL {
  uint64 Revision;
  EFI_FILE_OPEN Open;
  void *Close;
  void *Delete;
  void *Read;
  void *Write;
  void *GetPosition;
  void *SetPosition;
  EFI_FILE_GET_INFO GetInfo;
  void *SetInfo;
  void *Flush;
  void *OpenEx;
  void *AReadEx;
  void *WriteEx;
  void *FlushEx;
} EFI_FILE_PROTOCOL;

typedef struct {
  uint32 Revision;
  EFI_HANDLE ParentHandle;
  EFI_SYSTEM_TABLE *SystemTable;

  EFI_HANDLE DeviceHandle;
  void /*EFI_DEVICE_PATH_PROTOCOL*/ *FilePath;
  void *Reserved;

  uint32 LoadOptionSize;
  void *LoadOptions;

  void *ImageBase;
  uint64 ImageSize;
  EFI_MEMORY_TYPE ImageCodeType;
  EFI_MEMORY_TYPE ImageDataType;
  void * /*EFI_IMAGE_UNLOAD*/ Unload;
} EFI_LOADED_IMAGE_PROTOCOL;

struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

typedef EFI_STATUS (*EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME)(
    struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This, EFI_FILE_PROTOCOL **Root);

typedef struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
  uint64 Revision;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME OpenVolume;
} EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

typedef EFI_STATUS (*EFI_GET_MEMORY_MAP)(uintn *MemoryMapSize,
                                         EFI_MEMORY_DESCRIPTOR *MemoryMap,
                                         uintn *MapKey, uintn *DescriptorSize,
                                         uintn *DescriptorVersion);

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

typedef struct EFI_BOOT_SERVICES {
  ///
  /// The table header for the EFI Boot Services Table.
  ///
  EFI_TABLE_HEADER Hdr;

  //
  // Task Priority Services
  //
  void *RaiseTPL;
  void *RestoreTPL;

  //
  // Memory Services
  //
  void *AllocatePages;
  void *FreePages;
  EFI_GET_MEMORY_MAP GetMemoryMap;
  void *AllocatePool;
  EFI_FREE_POOL FreePool;

  //
  // Event & Timer Services
  //
  void *CreateEvent;
  void *SetTimer;
  void *WaitForEvent;
  void *SignalEvent;
  void *CloseEvent;
  void *CheckEvent;

  //
  // Protocol Handler Services
  //
  void *InstallProtocolInterface;
  void *ReinstallProtocolInterface;
  void *UninstallProtocolInterface;
  void *HandleProtocol;
  void *Reserved;
  void *RegisterProtocolNotify;
  void *LocateHandle;
  void *LocateDevicePath;
  void *InstallConfigurationTable;

  //
  // Image Services
  //
  void *LoadImage;
  void *StartImage;
  void *Exit;
  void *UnloadImage;
  EFI_EXIT_BOOT_SERVICES ExitBootServices;

  //
  // Miscellaneous Services
  //
  void *GetNextMonotonicCount;
  void *Stall;
  void *SetWatchdogTimer;

  //
  // DriverSupport Services
  //
  void *ConnectController;
  void *DisconnectController;

  //
  // Open and Close Protocol Services
  //
  EFI_OPEN_PROTOCOL OpenProtocol;
  void *CloseProtocol;
  void *OpenProtocolInformation;

  //
  // Library Services
  //
  void *ProtocolsPerHandle;
  EFI_LOCATE_HANDLE_BUFFER LocateHandleBuffer;
  void *LocateProtocol;
  void *InstallMultipleProtocolInterfaces;
  void *UninstallMultipleProtocolInterfaces;

  //
  // 32-bit CRC Services
  //
  void *CalculateCrc32;

  //
  // Miscellaneous Services
  //
  void *CopyMem;
  void *SetMem;
  void *CreateEventEx;
} EFI_BOOT_SERVICES;

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL 0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL 0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL 0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER 0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE 0x00000020

#define EFI_SUCCESS 0

#endif
