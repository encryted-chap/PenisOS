#pragma once
#include "Core/Types.hpp"
#include "System/PIC.h"
#include "Core/Console.h"

class VirtualReg {
public:
    unsigned long val;
    
    // call this to save changes
    void Set() {
        val = temp_val;
    }
private:

    VirtualReg *operator=(unsigned long v) { 
        temp_val = v;
        return this; 
    }
    VirtualReg *operator+=(unsigned long v) {
        temp_val += v;
        return this;
    }
    VirtualReg *operator-=(unsigned long v) {
        temp_val -= v;
        return this;
    }
    unsigned long temp_val;
};
class Thread {
public:
    unsigned char *b;
    Thread(unsigned char *bytecode) {
        b = bytecode;
        initialized = true;
    }
    // executes a single instruction
    void Step() {
        switch (b[index]) {
            case 0xFF: { 
                // TERM 
                initialized = false;
            } break;
        }
        index++;
        for(int i = 0; i < 3; i++) 
            r[i].Set();
    }
    bool Active() {

    }
    operator bool() {
        return initialized;
    }
private:
    VirtualReg r[3];
    int index;

    bool initialized = false;
};

class _ThreadMan {
public:
    Thread *regt[256];
    int index;
    bool free[256];

    _ThreadMan() {
        for(int i = 0; i < 256; i++) 
            free[i] = true;
    }

    // registers a thread to the manager
    void Add(Thread* T) {
        for(int i = 0; i < 256; i++) {
            if(free[i]) {
                regt[i] = T;
                return;
            }
        }
    }
    void PIT_Fire() {
        if(index == 255) 
            index = 0;
        for(int i = 0; i < 256; i++) {
            if(regt[index]->Active())
                regt[index++]->Step();
        }
    }
}; _ThreadMan ThreadMan;