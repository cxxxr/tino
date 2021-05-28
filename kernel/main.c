#include "frame_buffer.h"
#include "font.h"

#define COLOR_RED(color) ((color >> 16) & 0xff)
#define COLOR_GREEN(COLOR) ((color >> 8) & 0xff)
#define COLOR_BLUE(COLOR) (color & 0xff)

void draw_pixel(FrameBuffer * frame_buffer, int x, int y, uint64 color)
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

    int offset = (y * frame_buffer->pixels_per_scan_line + x) * 4;
    frame_buffer->frame_buffer_base[offset + r] = COLOR_RED(color);
    frame_buffer->frame_buffer_base[offset + g] = COLOR_GREEN(color);
    frame_buffer->frame_buffer_base[offset + b] = COLOR_BLUE(color);
}

void draw_char(FrameBuffer * frame_buffer, int x, int y, unsigned char c, uint64 color)
{
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 8; j++) {
            if ((font[c][i] >> (7 - j)) & 1) {
                draw_pixel(frame_buffer, x + j, y + i, color);
            }
        }
    }
}

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

    int x = 10, y = 10;
    for (unsigned char c = 0; c < 128; c++) {
        if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
            ('0' <= c && c <= '9')) {
            draw_char(frame_buffer, x, y, c, 0x000000);
            x += 8;
        }
    }

    while (1)
        __asm__("hlt");
}
