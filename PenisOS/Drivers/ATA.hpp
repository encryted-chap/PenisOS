#pragma once
#include "Core/Types.hpp"
#include "Core/IO.hpp"

// the maximum possible LBA28 address
#define LBA28_CAP 0x0FFFFFFF

// an ambiguous class for handling ATA access. This will cycle through each
// ATA bus depending on if the last one is full or not
class ATA_Bus {
public:
    ATA_Bus() {
        busIndex = -1; // make sure bus index is able to iterate
        SwitchBus(); // switches to next working bus
    }
private:
    // switches the current bus to the next bus
    void SwitchBus() {
        bool WorkingBus = false;
        while(!WorkingBus) {
            // iterate through to next bus offset
            CTRL_Base = ATA_CTRL[++busIndex];
            IO_Base = ATA_IO[busIndex];
            if(inb(IO_Base+7) == 0xFF) { // read regular status byte for floating bus
                // bus invalid, no drives connected
                WorkingBus = false;
                continue;
            } else {
                // continue to identify the drives connected
                // Set drive types
                Master.DType = Drive::Master;
                Slave.DType = Drive::Slave;
                // do identify drive operation to detect
                Identify(&Master);
                Identify(&Slave);

                // check if neither working
                if(!Master.Active && !Slave.Active) 
                    continue; // switch to next bus
                WorkingBus = true; // bus has a drive connected, cool?
            }
        }
    }
    
    
    int busIndex = 0; // used for controlling which bus is currently active
    // a structure representing a single drive
    struct Drive {
    public:
        enum DriveType {
            Master=0xA0, 
            Slave=0xB0
        };
        DriveType DType;
        bool Active;
        // information returned on the last IDENTIFY command
        uint16_t* identify_return;
        bool Supports48, Supports28;
        int MaxUDMA;
        int CurrentUDMA;
        uint32_t Max28;
        uint64_t Max48;
        unsigned char slavebit = 8U;
        void ParseIdentify() {

        }
        // sets the slavebit depending on the current drive type
        void SetSlavebit() {
            if(DType == Master) {
                slavebit = 0U;
            } else slavebit = 8U; // 00001000 aka set the slavebit
        }
    };
    Drive Master, Slave; // is this racist? i just follow the specs i don't make them :/

    // identifies a drive, returning the information to the drives pointer
    void Identify(Drive* toSelect) {
        outb(IO_Base+6, toSelect->DType); // send drive type to drive select port
        toSelect->SetSlavebit(); // make sure drive slavebit is correct
        // 400ns delay:
        uint8_t status;
        for(int i = 0; i < 14; i++) {
            status = inb(IO_Base+7); // read data for delay
        }
        // clear sectorcount-lbahi ports
        for(int i = 0; i < 4; i++) {
            int offset = i+2; // starts at 0x1F2, so offset = i+2
            outb(IO_Base+offset, 0x00); // clear register
        }
        outb(IO_Base+7, 0xEC); // send identify command

        if(inb(IO_Base+7) == 0) {
            // drive does not exist
            toSelect->Active = false;
        } else {
            // wait for BSY to clear
            while(inb(IO_Base+7) == 8) {
                // if nonzero dont poll
                if(inb(IO_Base+4) && inb(IO_Base+5)) {
                    toSelect->Active = false; // not ATA drive
                    return;
                }
                if(inb(IO_Base+7) == 1) {
                    // if ERR sets, quit
                    toSelect->Active = false;
                    return;
                }
            }
            toSelect->Active = true; // this drive is working

            // now we can read and return 256 uint16_t values
            for(int i = 0; i < 256; i++) 
                toSelect->identify_return[i] = inw(IO_Base);
            for(int i = 0; i < 14; i++) {
                inb(IO_Base+7); // 400ns delay (in case there's another select right after this one)s
            }
            toSelect->ParseIdentify(); // make sure to retrieve correct data
            return; // end identify
        }
        
    }
    uint8_t ATA_IO[4] {
        0x1F0, 0x170, 0x1E8, 0x168
    };
    uint8_t ATA_CTRL[4] {
        0x3F6, 0x376, 0x3E6, 0x366
    };
    unsigned char IO_Base = 0x00;
    unsigned char CTRL_Base = 0x00;

};