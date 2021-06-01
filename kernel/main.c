#include "frame_buffer.h"
#include "asmfunc.h"
#include "serial.h"

void KernelMain(FrameBuffer * frame_buffer)
{
    draw_rectangle(frame_buffer, 0, 0,
                   frame_buffer->horizontal_resolution,
                   frame_buffer->vertical_resolution, 0xffffff);

    init_serial_ports();

    serial_write_string("Hello World");

    while (1) __asm__("hlt");
}
