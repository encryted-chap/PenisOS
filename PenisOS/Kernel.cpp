#include "Drivers/ATA.hpp"
#include "Core/Types.hpp"
extern "C" void Entry();

// the entry point of PenisOS
// why am i doing this what the fuck
void Entry() {
    ATA_Bus bus = ATA_Bus();
    ((char*)0xb8000)[0] = 'h';
}