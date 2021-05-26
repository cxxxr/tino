#include "frame_buffer.h"

#define COLOR_RED(color) ((color >> 16) & 0xff)
#define COLOR_GREEN(COLOR) ((color >> 8) & 0xff)
#define COLOR_BLUE(COLOR) (color & 0xff)

void draw_rectangle(FrameBuffer * frame_buffer, int x, int y, int width,
                    int height, uint64 color)
{
    int r, g, b;

    switch (frame_buffer->pixel_format) {
    case PIXEL_RED_GREEN_BLUE_RESERVED_8BIT_PER_COLOR:
        r = 0;
        g = 1;
        b = 2;
        break;
    case PIXEL_BLUE_GREEN_RED_RESERVED_8BIT_PER_COLOR:
        b = 0;
        g = 1;
        r = 2;
        break;
    default:
        break;
    }

    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            int offset = (i * frame_buffer->pixels_per_scan_line + j) * 4;
            frame_buffer->frame_buffer_base[offset + r] = COLOR_RED(color);
            frame_buffer->frame_buffer_base[offset + g] =
                COLOR_GREEN(color);
            frame_buffer->frame_buffer_base[offset + b] =
                COLOR_BLUE(color);
        }
    }
}

void KernelMain(FrameBuffer * frame_buffer)
{
    draw_rectangle(frame_buffer, 0, 0,
                   frame_buffer->horizontal_resolution,
                   frame_buffer->vertical_resolution, 0xffffff);
    draw_rectangle(frame_buffer, 100, 50, 300, 400, 0x00ff00);

    while (1)
        __asm__("hlt");
}
