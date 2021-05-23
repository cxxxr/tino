typedef unsigned long long int uint64;
typedef unsigned char uint8;

void KernelMain(uint64 frame_buffer_base, uint64 frame_buffer_size) {
    uint8* frame_buffer = (uint8*)frame_buffer_base;
    for (uint64 i = 0; i < frame_buffer_size; ++i) {
        frame_buffer[i] = i % 256;
    }
    while (1) __asm__("hlt");
}
