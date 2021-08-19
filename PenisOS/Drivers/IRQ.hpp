#pragma once

#include "Core/IO.hpp"
#include "Core/Types.hpp"

#define Master_CMD 0x0020
#define Master_DATA 0x0021
#define Slave_CMD 0x00A0
#define Slave_DATA 0x00A1
#define READ_ISR 0x0B
#define READ_IRR 0x0A

int CMD_MODE = 0;
// class for interfacing with the Programmable Interrupt Controller
class PIC {
public:
    // sends the end of interrupt command to a PIC
    static void EndInterrupt(unsigned char IRQ_ID) {
        // if this is >= 8 send to the secondary PIC
        if(IRQ_ID >= 8) {
            outb(Slave_CMD, 0x20); // send EOI to Slave
        } 
        outb(Master_CMD, 0x20); // make sure to send to both
    }
    // disables the PIC
    static void Disable_PIC() {
        outb(Master_DATA, 0xFF); // shut down command to master
        outb(Slave_DATA, 0xFF); // shut down command to slave
    }
    // waits a few microseconds to help not fry the PIC
    static inline void io_wait(void)
    {
        outb(0x80, 0);
    }
    static uint16_t ReadIRR() {
        // since the PIC remembers our last switch, only do so if it's not mode 1
        if(CMD_MODE != READ_IRR) {
            // send command to receive IRR to the cmd regs
            outb(Master_CMD, READ_IRR);
            outb(Slave_CMD, READ_IRR); 
        }
        CMD_MODE = READ_IRR; // IRR mode
        // now append the two regs into one value
        return (inb(Slave_CMD) << 8) | inb(Master_CMD); // now return
    }
    static uint16_t ReadISR() {
        if(CMD_MODE != READ_ISR) {
            // send command
            outb(Master_CMD, READ_ISR);
            outb(Slave_CMD, READ_ISR);
        }
        CMD_MODE = READ_ISR; // ISR mode
        // now do the same thing as ReadIRR(void)
        return (inb(Slave_CMD) << 8) | inb(Master_CMD);
    }
};
// general IRQ class
class IRQ_Amb {
public:

};