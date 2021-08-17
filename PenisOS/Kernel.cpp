#include "Drivers/ATA.hpp"
#include "Core/Types.hpp"
#include "Core/Console.hpp"
extern "C" void Entry();

// the entry point of PenisOS
// why am i doing this what the fuck
void Entry() {
    // initialize the console
    Console::Initialize();
    Console::KernelMsg("Initializing ATA driver...");

    ATA_Bus bus = ATA_Bus(); // create new ATA ambiguous driver
    if(bus.Master.Active || bus.Slave.Active) {
        Console::SuccessMsg("ATA initialized, at least 1 drive active");
    } else return;
}