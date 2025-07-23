#include <cstdint>
#include <inc/io/serial.hpp>

bool isSerialEnabled;

static inline void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

int serialInit()
{
    outb(PORT + 1, 0x00);
    outb(PORT + 3, 0x80);
    outb(PORT + 0, 0x03);
    outb(PORT + 1, 0x00);
    outb(PORT + 3, 0x03);
    outb(PORT + 2, 0xC7);
    outb(PORT + 4, 0x0B);
    outb(PORT + 4, 0x1E);
    outb(PORT + 0, 0xAE);

    if (inb(PORT + 0) != 0xAE)
    {
        isSerialEnabled = false;
        return 1;
    }

    outb(PORT + 4, 0x0F);

    isSerialEnabled = true;
    return 0;
}

int is_transmit_empty() {
   return inb(PORT + 5) & 0x20;
}

void writeSerial(char a) {
    if (!isSerialEnabled) {
        return;
    }
   while (is_transmit_empty() == 0);

   outb(PORT,a);
}

void stringToSerial(char *string)
{
    if (!isSerialEnabled) {
        return;
    }
    std::uint64_t i = 0;
    while(string[i] != '\0')
    {
        writeSerial(string[i]);
        i++;
    }
}