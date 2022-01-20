#pragma once
#define PRI_CMD 0x0020
#define PRI_DAT 0x0021
#define SEC_CMD 0x00A0
#define SEC_DAT 0x00A1
#define PIC_EOI 0x20
#define ICW1_ICW4	0x01	
#define ICW1_SINGLE	0x02	
#define ICW1_INTERVAL4	0x04	
#define ICW1_LEVEL	0x08	
#define ICW1_INIT	0x10	
 
#define ICW4_8086	0x01
#define ICW4_AUTO	0x02	
#define ICW4_BUF_SLAVE	0x08	
#define ICW4_BUF_MASTER	0x0C		
#define ICW4_SFNM	0x10
#define PIC1 0x20
#define PIC2 0xA0

#define ICW1 0x11
#define ICW4 0x01

#include "Core/HMemory.h"

class PIC {
public:
    static void IRQ_set_mask(unsigned char IRQline) {
        uint16_t port;
        uint8_t value;
    
        if(IRQline < 8) {
            port = PRI_DAT;
        } else {
            port = SEC_DAT;
            IRQline -= 8;
        }
        value = inb(port) | (1 << IRQline);
        outb(port, value);        
    }
    
    static void IRQ_clear_mask(unsigned char IRQline) {
        uint16_t port;
        uint8_t value;
    
        if(IRQline < 8) {
            port = PRI_DAT;
        } else {
            port = SEC_DAT;
            IRQline -= 8;
        }
        value = inb(port) & ~(1 << IRQline);
        outb(port, value);        
    }
    static void SendEOI(unsigned char irq_num) {
        if(irq_num >= 8)
            outb(SEC_CMD, PIC_EOI);
        outw(PRI_CMD, PIC_EOI);
    }
    static void Remap(int pic1, int pic2) {
        /* send ICW1 */
        outb(PIC1, ICW1);
        outb(PIC2, ICW1);

        /* send ICW2 */
        outb(PIC1 + 1, pic1);	/* remap */
        outb(PIC2 + 1, pic2);	/*  pics */

        /* send ICW3 */
        outb(PIC1 + 1, 4);	/* IRQ2 -> connection to slave */
        outb(PIC2 + 1, 2);

        /* send ICW4 */
        outb(PIC1 + 1, ICW4);
        outb(PIC2 + 1, ICW4);

        /* disable all IRQs */
        outb(PIC1 + 1, 0xFF);
    }
};