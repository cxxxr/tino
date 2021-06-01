#ifndef FRAME_BUFFER_H_
#define FRAME_BUFFER_H_

#include "efi.h"

typedef enum {
    PIXEL_RED_GREEN_BLUE_RESERVED_8BIT_PER_COLOR,
    PIXEL_BLUE_GREEN_RED_RESERVED_8BIT_PER_COLOR,
} PixelFormat;

typedef struct _FrameBuffer {
    PixelFormat pixel_format;
    uint32 pixels_per_scan_line;
    uint32 horizontal_resolution;
    uint32 vertical_resolution;
    uint8 *frame_buffer_base;
    uintn frame_buffer_size;
} FrameBuffer;

void draw_pixel(FrameBuffer * frame_buffer, int x, int y, uint64 color);
void draw_char(FrameBuffer * frame_buffer, int x, int y, unsigned char c, uint64 color);
void draw_rectangle(FrameBuffer * frame_buffer, int x, int y, int width,
                    int height, uint64 color);

#endif
