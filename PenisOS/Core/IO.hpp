#pragma once
#include "Core/Types.hpp"

bool GetBit(byte value, int bit) {
    return ((value >> bit) & 0x01) != 0;
}
template<class T> bool GetBit(T value, int bit) {
    if(bit > sizeof(T) * 8) 
        return false;
    else return (value & (1 << bit)) != 0;
}
static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}
static inline void outw(uint16_t port, uint16_t val)
{
    asm volatile ( "outw %0, %1" : : "a"(val), "Nd"(port) );
}
static inline void outd(uint16_t port, uint32_t val)
{
    asm volatile ( "outd %0, %1" : : "a"(val), "Nd"(port) );
}
static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}
static inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    asm volatile ( "inw %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}
static inline uint32_t ind(uint16_t port)
{
    uint32_t ret;
    asm volatile ( "ind %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}
void memcpy(const unsigned char* src, unsigned char* dest, size_t len) {
    size_t og_len = len;
    while(len) {
        // if len > 8 bytes, copy a 64 bit value (8 bytes)
        if(len > sizeof(uint64_t)) {
            *(uint64_t*)dest = *(uint64_t*)src;
            dest += sizeof(uint64_t);
            src += sizeof(uint64_t);
            len -= sizeof(uint64_t);
        } else if(len > sizeof(uint32_t)) {
            // copy a 32 bit value (4 bytes)
            *(uint32_t*)dest = *(uint32_t*)src;
            dest += sizeof(uint32_t);
            src += sizeof(uint32_t);
            len -= sizeof(uint32_t);
        } else if(len > sizeof(uint16_t)) {
            *(uint16_t*)dest = *(uint16_t*)src;
            dest += sizeof(uint16_t);
            src += sizeof(uint16_t);
            len -= sizeof(uint16_t);
        } else {
            *dest++ = *src++;
            len--;
        }
    }
    src -= og_len;
    dest -= og_len;
    return;
}

template<class T> class IOStream {
public:
    IOStream(uint16_t address) {
        buffer = (T*)address; // initialize a pointer at the given address
    }
    T operator << (T other) {
        
    }
private:
    int index = 0;
    T* buffer;
};
