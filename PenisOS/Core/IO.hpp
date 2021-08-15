#pragma once
#include "Core/Types.hpp"
bool GetBit(byte value, int bit) {
    return ((value >> bit) & 0x01) != 0;
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
void memcpy(char* src, char* dest, uint16_t length) {
    register uint16_t* cx asm("cx");
    register uint16_t* si asm("si"); // from
    register uint16_t* di asm("di"); // to

    si = (uint16_t*)src;
    di = (uint16_t*)dest;
    cx[0] = length;
    asm("rep movsb");  
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
