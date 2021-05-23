#ifndef EFI_H_
#define EFI_H_

typedef unsigned long int uint32;
typedef unsigned long long int uint64;
typedef unsigned char uint8;
typedef uint64 uintn;

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
    _EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info;
    uintn frame_buffer_base;
    uintn frame_buffer_size;
} _EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

#endif
