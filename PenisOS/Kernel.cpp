#include "Drivers/ATA.hpp"
#include "Core/Types.hpp"
#include "Drivers/IRQ.hpp"
#include "Core/Console.hpp"
#include "Core/String.hpp"
extern "C" void Entry();

// the entry point of PenisOS
// why am i doing this what the fuck
void Entry() {
    // initialize the console
    Console::Initialize();
    Console::SuccessMsg("Boot success! Kernel initializing");
}