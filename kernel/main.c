#include "efi.h"

typedef struct _Drawer {
    uint8 *frame_buffer_base;
    uint32 pixels_per_scan_line;
    _EFI_GRAPHICS_PIXEL_FORMAT pixel_format;
} Drawer;

#define COLOR_RED(color) ((color >> 16) & 0xff)
#define COLOR_GREEN(COLOR) ((color >> 8) & 0xff)
#define COLOR_BLUE(COLOR) (color & 0xff)

void draw_rectangle(Drawer * drawer, int x, int y, int width, int height,
                    uint64 color)
{
    int r, g, b;

    switch (drawer->pixel_format) {
    case _PixelRedGreenBlueReserved8BitPerColor:
        r = 0;
        g = 1;
        b = 2;
        break;
    case _PixelBlueGreenRedReserved8BitPerColor:
        b = 0;
        g = 1;
        r = 2;
        break;
    default:
        break;
    }

    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            int offset = (i * drawer->pixels_per_scan_line + j) * 4;
            drawer->frame_buffer_base[offset + r] = COLOR_RED(color);
            drawer->frame_buffer_base[offset + g] = COLOR_GREEN(color);
            drawer->frame_buffer_base[offset + b] = COLOR_BLUE(color);
        }
    }
}

void KernelMain(_EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE mode)
{
    Drawer drawer = {
        (uint8 *) mode.frame_buffer_base,
        mode.info->pixels_per_scan_line,
        mode.info->pixel_format
    };

    draw_rectangle(&drawer, 100, 50, 300, 400, 0x00ff00);

    while (1)
        __asm__("hlt");
}
