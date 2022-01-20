#include "Core/Types.hpp"
#include "Core/HMemory.h"
#include "Core/Console.h"
#include "Core/Thread.h"
#include "System/IDT.h"
#include "System/GDT.h"
#include "System/PIC.h"
#include "Core/PIT_Handler.h"
#include "Devices/Keyboard.hpp"
#include "System/ATA.hpp"

extern "C" void Entry(void);

int x;

void Entry() {
    gdt_install();
    console::clearscreen();

    IDT::CreateEntry(8, (unsigned long)PIT::OnFire, 0x08, 0x8E);
    outb(0x40, 0);
    IDT::Load();
    console::Success("IDT successfully loaded");
    console::Success("IRQs unmasked");

    Drive x[2];
    ATA::Identify(x);
    if(x[0].scount_28)
        console::print("yup");
    else console::print("nope");
    asm("sti");
    PIC::IRQ_clear_mask(0);
    
    

    
}

