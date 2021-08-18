#pragma once
#include "Core/Types.hpp"
#include "Core/IO.hpp"
#include "Core/Console.hpp"

// the maximum possible LBA28 address
#define LBA28_CAP 0x0FFFFFFF
int CurrentID = 0;
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
        int bus = 0;
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
                if(!Master.Active && !Slave.Active) {
                    ++bus;
                    continue; // switch to next bus
                }
                else WorkingBus = true; // bus has a drive connected, cool?
                
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

        int DriveID; 
        unsigned char slavebit = 8U;
        void ParseIdentify() {
            DriveID = CurrentID++;
            Supports48 = GetBit<uint16_t>(identify_return[83], 10); // bit 10 of [83] is set if drive supports LBA48

            // maximum 28 bit LBA address
            Max28 = ((identify_return[61] >> 16) | identify_return[60]);
            if(Max28 > 0 && Max28 <= LBA28_CAP) Supports28 = true; // if max28 is nonzero, it supports 28 bit lba

            // maximum 48 bit LBA address
            Max48 = (uint64_t)((identify_return[103] >> 16) | identify_return[102]) | ((identify_return[101] >> 16) | identify_return[100]);
            if(Max48 > 0) Supports48 = true;
        }
        // sets the slavebit depending on the current drive type
        void SetSlavebit() {
            if(DType == Master) {
                slavebit = 0U;
            } else slavebit = 8U; // 00001000 aka set the slavebit
        }
        // prints a description of this drive
        char* DriveInfo() {
            if(!this->Active) return "";
            Console::Write('\n');
            Console::Write("--------------------");
            if(DType == DriveType::Master) Console::Write("\nDrive Type: Master");
            else Console::Write("\nDriveType: Slave");
            Console::Write("\nSupports 28-bit: ");
            Console::Write(Supports28);
            Console::Write("\nSupports 48-bit: ");
            Console::Write(Supports48);
            Console::Write("\nSize48: ");
            Console::Write((int)(Max48*512));
            Console::Write(" bytes");
            Console::Write("\nSize28: ");
            Console::Write((int)(Max28*512));
            Console::Write(" bytes");
            Console::Write("\nDriveID = ");
            Console::Write(DriveID);
            Console::Write('\n');
            // chimay's feet go here: 
        }
    };
    void CacheFlush() {
        outb(IO_Base+7, 0xE7); // cache flush to command register
        while(inb(IO_Base+7) == 0x80); // wait for BSY
    }
public:
    Drive Master, Slave; // is this racist? i just follow the specs i don't make them :/
    // writes to a drive using 28 bit LBA
    void Write28(Drive drive, uint32_t LBA, unsigned char sectorcount, uint16_t *data) {
        drive.SetSlavebit(); // make sure slavebit is accurate
        if(LBA+(sectorcount-1) > drive.Max28) return;

        outb(IO_Base+6, (0xD0 + drive.DType) | (drive.slavebit << 4) | ((LBA >> 24) & 0x0F)); // select a drive 
        for(int i = 0; i < 14; i++) 
            inb(IO_Base+7); // 400ns delay after selecting a drive
        outb(IO_Base+2, sectorcount); // send sectorcount to drive
        outb(IO_Base+3, (unsigned char)LBA); // send low 8 bits of LBA
        outb(IO_Base+4, (unsigned char)LBA >> 8); // send mid 8 bits
        outb(IO_Base+5, (unsigned char)LBA >> 16); // send high 8 bits

        outb(IO_Base+7, 0x30); // WRITE SECTORS command

        for(int i = 0; i < 14; i++) inb(IO_Base+7); // 400ns delay

        for(int i = 0; i < sectorcount * 256; i++) {
            if(i != 0 && !(i % 256)) {
                CacheFlush();
                for(int x = 0; x < 14; x++) inb(IO_Base+7); // 400ns delay
            }
            outw(IO_Base, data[i]); // feed data to disk
            asm("jmp .+2"); // tiny delay between every write command
        }
        for(int i = 0; i < 14; i++) inb(IO_Base+7); // 400ns delay
        CacheFlush(); // clear to prevent bad sectors
    }
    uint16_t* Read28(Drive drive, uint32_t LBA, unsigned char sectorcount) {
        drive.SetSlavebit(); // make sure slavebit is accurate
        if(LBA+(sectorcount-1) > drive.Max28) return (uint16_t*)"ERR:OVMAX"; // return error buffer

        outb(IO_Base+6, (0xD0 + drive.DType) | (drive.slavebit << 4) | ((LBA >> 24) & 0x0F)); // select a drive 
        for(int i = 0; i < 14; i++) 
            inb(IO_Base+7); // 400ns delay after selecting a drive

        outb(IO_Base+2, sectorcount); // send sectorcount to drive
        outb(IO_Base+3, (unsigned char)LBA); // send low 8 bits of LBA
        outb(IO_Base+4, (unsigned char)LBA >> 8); // send mid 8 bits
        outb(IO_Base+5, (unsigned char)LBA >> 16); // send high 8 bits

        outb(IO_Base+7, 0x20); // READ SECTORS command
        for(int i = 0; i < 14; i++) inb(IO_Base+7); // 400ns delay

        uint16_t* ret;
        for(int i = 0; i < sectorcount * 256; i++) {
            if(i != 0 && !(i % 256)) {
                for(int x = 0; x < 14; x++) inb(CTRL_Base); // 400ns delay every sector
            }
            ret[i] = inw(IO_Base); // get single word from data buffer
        }
        for(int x = 0; x < 14; x++) inb(CTRL_Base); // 400ns delay
        CacheFlush(); // make sure to clear after drive operation
        return ret;
    }
private:
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
            return; 
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
            for(int i = 0; i < 14; i++) inb(IO_Base+7); // 400ns delay

            // now we can read and return 256 uint16_t values
            for(int i = 0; i < 256; i++) {
                toSelect->identify_return[i] = inw(IO_Base);
                if(!toSelect->Active && toSelect->identify_return[i] != 0) 
                    toSelect->Active = true;
            }
            for(int i = 0; i < 14; i++) {
                inb(IO_Base+7); // 400ns delay (in case there's another select right after this one)s
            }
            toSelect->ParseIdentify(); // make sure to retrieve correct data
            return; // end identify
        }
        
    }
    // base IO ports for ATA buses
    uint8_t ATA_IO[4] {
        0x1F0, 0x170, 0x1E8, 0x168
    };
    // base Control ports for ATA buses
    uint8_t ATA_CTRL[4] {
        0x3F6, 0x376, 0x3E6, 0x366
    };
    // the current IO base for this ATA bus
    unsigned char IO_Base = 0x00; 
    // the current Control base for this ATA bus
    unsigned char CTRL_Base = 0x00;

};