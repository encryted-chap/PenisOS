#pragma once
#include "Core/Types.hpp"
#include "Core/HMemory.h"

#define ATA_DATA 0x0
#define ATA_ERROR 0x1
#define ATA_FEATURES 0x1
#define ATA_SC 0x2
#define ATA_SN 0x3
#define CYL_LO 0x4
#define CYL_HI 0x5
#define DRIVE_REG 0x6
#define ATA_STATUS 0x7
#define ATA_CMD 0x7
enum BusCTRL {

};
enum BusIO {
    primary=0x1F0
};
struct Drive {
    bool exists;
    int slave; // set if drive is slave
    bool LBA48; // set if lba48 supported
    bool LBA28;

    // sector counts for 48 and 28 bit LBA modes
    uint32_t scount_28;
    uint64_t scount_48;

    BusCTRL ctrl_base;
    BusIO io_base=primary;

    void ParseStatus(uint16_t status[256], Drive *d) {
        LBA48 = (bool)(status[83] & (1 << 10)); // get lba48
        scount_28 = *(uint32_t*)&status[60];
        scount_48 = *(uint64_t*)&status[100];
        if(!scount_28 && !scount_48)
            exists = false;
        
        d->LBA48 = LBA48;
        d->scount_28 = scount_28;
        d->scount_48 = scount_48;
        d->exists = exists;
    }
};

class ATA {
public:
    static void Identify(Drive drives[2]) {
        drives[0].exists = true;
        drives[1].exists = true;
        for(int i = 0xA0, x = 0; i != 0xC0; i += 0x10, x++) {
            if(i == 0xB0) drives[x].slave = 1;
            outb(drives[x].io_base+DRIVE_REG, (uint16_t)i); // select correct register
            Poll(drives[x]); 
            // now set io+2 - io+5 to 0
            for(int y = 2; y < 6; y++)
                outb(drives[x].io_base+y, 0x0); // set 0
            outb(drives[x].io_base+ATA_CMD, 0xEC); // send IDENTIFY command

            (&drives[x])->exists = (bool)inb(drives[i].io_base+ATA_STATUS);
            
            uint16_t status[256];
            for(int y = 0; y < 256; y++) {
                status[y] = inw(drives[x].io_base);
            }
            Drive().ParseStatus(status, &(drives[x]));
        }
        
    }
    static uint8_t Poll(Drive drive) {
        uint8_t ret = 0x0;
        for(int i = 0; i < 14; i++) {
            ret = inb(drive.io_base+ATA_STATUS);
        }
        return ret;
    }
};