#include "efi.h"

void KernelMain(_EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE mode) {
    uint8* frame_buffer = (uint8*)(mode.frame_buffer_base);
    for (uint64 i = 0; i < mode.frame_buffer_size; ++i) {
        frame_buffer[i] = i % 256;
    }
    while (1) __asm__("hlt");
}
