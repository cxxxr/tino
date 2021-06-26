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
} _EFI_TABLE_HEADER;

typedef void *_EFI_HANDLE;
typedef void *_EFI_EVENT;

typedef struct {
  void *Reset;
  void *ReadKeyStroke;
  _EFI_EVENT WaitForKey;
} _EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef EFI_STATUS (*_EFI_TEXT_STRING)(
    struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, CHAR16 *String);

typedef EFI_STATUS (*_EFI_TEXT_SET_CURSOR_POSITION)(
    struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, uintn column, uintn row);

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
  void *Reset;
  _EFI_TEXT_STRING OutputString;
  void *TestString;
  void *QueryMode;
  void *SetMode;
  void *SetAttribute;
  void *ClearScreen;
  _EFI_TEXT_SET_CURSOR_POSITION SetCursorPosition;
  void *EnableCursor;
  void *Mode;
} _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

struct _EFI_BOOT_SERVICES;

typedef struct {
  _EFI_TABLE_HEADER Hdr;
  char16 *FirmwareVendor;
  uint32 FirmwareRevision;
  _EFI_HANDLE ConsoleInHandle;
  _EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;
  _EFI_HANDLE ConsoleOutHandle;
  _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
  _EFI_HANDLE StandardErrorHandle;
  _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdErr;
  void *RuntimeServices;
  struct _EFI_BOOT_SERVICES *BootServices;
  uintn NumberOfTableEntries;
  void *ConfigurationTable;
} _EFI_SYSTEM_TABLE;

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
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef struct {
  void * /*EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE*/ QueryMode;
  void * /*EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE*/ SetMode;
  void * /*EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT*/ Blt;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;
} EFI_GRAPHICS_OUTPUT_PROTOCOL;

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

struct _EFI_FILE_PROTOCOL;

typedef EFI_STATUS (*EFI_FILE_OPEN)(struct _EFI_FILE_PROTOCOL *This,
                                    struct _EFI_FILE_PROTOCOL **NewHandle,
                                    char16 *FileName, uint64 OpenMode,
                                    uint64 Attributes);

typedef EFI_STATUS (*EFI_FILE_GET_INFO)(struct _EFI_FILE_PROTOCOL *This,
                                        EFI_GUID *InformationType,
                                        uintn *BufferSize, void *Buffer);

typedef struct _EFI_FILE_PROTOCOL {
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
  _EFI_HANDLE ParentHandle;
  _EFI_SYSTEM_TABLE *SystemTable;

  _EFI_HANDLE DeviceHandle;
  void /*EFI_DEVICE_PATH_PROTOCOL*/ *FilePath;
  void *Reserved;

  uint32 LoadOptionSize;
  void *LoadOptions;

  void *ImageBase;
  uint64 ImageSize;
  _EFI_MEMORY_TYPE ImageCodeType;
  _EFI_MEMORY_TYPE ImageDataType;
  void * /*EFI_IMAGE_UNLOAD*/ Unload;
} EFI_LOADED_IMAGE_PROTOCOL;

struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

typedef EFI_STATUS (*EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME)(
    struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This, EFI_FILE_PROTOCOL **Root);

typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
  uint64 Revision;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME OpenVolume;
} EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

typedef EFI_STATUS (*EFI_GET_MEMORY_MAP)(uintn *MemoryMapSize,
                                         _EFI_MEMORY_DESCRIPTOR *MemoryMap,
                                         uintn *MapKey, uintn *DescriptorSize,
                                         uintn *DescriptorVersion);

typedef EFI_STATUS (*EFI_EXIT_BOOT_SERVICES)(_EFI_HANDLE, uintn);

typedef enum {
  AllHandles,
  ByRegisterNotify,
  ByProtocol
} EFI_LOCATE_SEARCH_TYPE;

typedef EFI_STATUS (*EFI_LOCATE_HANDLE_BUFFER)(
    EFI_LOCATE_SEARCH_TYPE SearchType, EFI_GUID *Protocol, void *SearchKey,
    uintn *NoHandles, _EFI_HANDLE **Buffer);

typedef EFI_STATUS (*EFI_OPEN_PROTOCOL)(_EFI_HANDLE Handle, EFI_GUID *Protocol,
                                        void **Interface,
                                        _EFI_HANDLE AgentHandle,
                                        _EFI_HANDLE ControllerHandle,
                                        uint32 Attributes);

typedef EFI_STATUS (*EFI_FREE_POOL)(void *Buffer);

typedef struct _EFI_BOOT_SERVICES {
  ///
  /// The table header for the EFI Boot Services Table.
  ///
  _EFI_TABLE_HEADER Hdr;

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
} _EFI_BOOT_SERVICES;

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL 0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL 0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL 0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER 0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE 0x00000020

#define EFI_SUCCESS 0

#endif
