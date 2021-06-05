#include "serial.h"
#include "asmfunc.h"

// https://wiki.osdev.org/Serial_Ports

const uint16 PORT = 0x3f8;
const uint16 PORT2 = 0x2f8;

static void init_serial(const uint16 port)
{
    write_io_port8(port + 1, 0x00);       // Disable all interrupts
    write_io_port8(port + 3, 0x80);       // Enable DLAB (set baud rate divisor)

    write_io_port8(port + 0, 0x03);       // Set divisor to 3 (lo byte) 38400 baud
    write_io_port8(port + 1, 0x00);       //                  (hi byte)

    write_io_port8(port + 3, 0x03);       // 8 bits, no parity, one stop bit
    write_io_port8(port + 2, 0xC7);       // Enable FIFO, clear them, with 14-byte threshold
    write_io_port8(port + 4, 0x0B);       // IRQs enabled, RTS/DSR set
}

void init_serial_ports(void)
{
    init_serial(PORT);
    init_serial(PORT2);
}

static int serial_received(void)
{
    return read_io_port8(PORT2 + 5) & 1;
}

char serial_read_char(void)
{
    while (!serial_received());
    return read_io_port8(PORT2);
}

static int is_transmit_empty(void)
{
    return read_io_port8(PORT + 5) & 0x20;
}

void serial_write_char(char a)
{
    while (!is_transmit_empty());

    write_io_port8(PORT, a);
}

void serial_write_string(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++) {
        serial_write_char(str[i]);
    }
}

void print_uint64_with_padding(uint64 value, int width)
{
    char buffer[21];
    int i = 20, column = 0;
    for (;;) {
        buffer[--i] = (value % 10) + '0';
        column++;
        value /= 10;
        if (value == 0) break;
    }
    while (width-- > column) {
        buffer[--i] = ' ';
    }
    buffer[20] = '\0';
    serial_write_string(buffer + i);
}

void print_uint64(uint64 value)
{
    print_uint64_with_padding(value, 0);
}
