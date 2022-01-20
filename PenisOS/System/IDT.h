/*=============================
*   The file containing code for 
*   the GDT and the IDT.  
=============================*/
#pragma once 

struct IDT_Entry {
    unsigned short base_lo;
    unsigned short sel;
    unsigned char always0;     
    unsigned char flags;
    unsigned short base_hi;
}__attribute__((packed)); // this removes 32-bit padding
struct IDT_Ptr {
    unsigned short limit;
    unsigned int base;
}__attribute__((packed));

IDT_Entry idt[256];
IDT_Ptr idtptr;

class IDT {
public:
    static void CreateEntry(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags) {
        IDT_Entry* ret = &idt[num];
        idt[num].flags = flags;
        idt[num].sel = sel;
        idt[num].always0 = 0; 
        idt[num].base_lo = (base & 0xFFFF);
        idt[num].base_hi = (base >> 16) & 0xFFFF;
    }
    
    static void Load() {
        idtptr.base = &idt;
        idtptr.limit = (sizeof(IDT_Entry) * 256) - 1; // idt limit -1

        asm (
            "lidt %0" :: "m"(idtptr)
        );
    }
};

