#include "frame_buffer.h"
#include "font.h"
#include "asmfunc.h"

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

// https://wiki.osdev.org/Serial_Ports

const uint16 PORT = 0x3f8;
// const uint16 PORT = 0x2f8;

int init_serial() {

    out8(PORT + 1, 0x00);    // Disable all interrupts
    out8(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)

    out8(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    out8(PORT + 1, 0x00);    //                  (hi byte)

    out8(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    out8(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    out8(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set

    return 0;
}

int serial_received()
{
    return in8(PORT + 5) & 1;
}

char read_serial()
{
    while (!serial_received());
    return in8(PORT);
}

int is_transmit_empty() {
   return in8(PORT + 5) & 0x20;
}

void write_serial_char(char a) {
   while (!is_transmit_empty());

   out8(PORT,a);
}

void write_serial_string(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        write_serial_char(str[i]);
    }
}

void KernelMain(FrameBuffer * frame_buffer)
{
    draw_rectangle(frame_buffer, 0, 0,
                   frame_buffer->horizontal_resolution,
                   frame_buffer->vertical_resolution, 0xffffff);

    init_serial();

    write_serial_string("Hello World");

    while (1) __asm__("hlt");
}
