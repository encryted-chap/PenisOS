/*==========================
*   The header file the keyboard.
*   What'd you expect?
=========================*/
#pragma once
#include "Core/HMemory.h"

class Keyboard {
public:
    static void Initialize() {
        outb(0x60, 0xF4); // enable scancodes
    }
};