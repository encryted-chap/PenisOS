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
        // switch to first bus
        IO_Base = ATA_IO::Primary_IO;
        CTRL_Base = ATA_CTRL::Primary_C;
        
        // identify and activate the drives
        Master = Identify(Drive::DriveType::Master);
        Slave = Identify(Drive::DriveType::Slave);
        
    }
    // flushes the cache on the currently selected ATA bus
    void FlushCache() {
        // send cache flush cmd to command register
        outb(IO_Base+7, 0xE7);

        // wait for BSY to clear
        while(GetBit(inb(CTRL_Base+0), 7)); 
    }
private:
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

        void SetSlavebit() {
            if(DType == Master) {
                slavebit = 0U;
            } else slavebit = 8U; // 00001000 aka set the slavebit
        }
    };
    Drive Master;
    Drive Slave;
    Drive Identify(Drive::DriveType Type) {
        Drive Ret;
        Ret.DType = Type;

        SelectDrive(Type);
        ClearLBA(); // set LBA vals to 0
        outb(IO_Base+7, 0xEC); // send IDENTIFY command

        // if value reads 0, drive does not exist
        if(inb(IO_Base+7) == 0) {
            Ret.Active = false;
            Ret.Supports48 = false;
            Ret.Supports28 = false;
        } else {
            if(GetBit(inb(IO_Base+7), 0)) {
                Ret.Active = true;
                goto readvals;
            }
                
            // poll
            for(int i = 0; i < 9000; i++) {
                if(!GetBit(inb(IO_Base+7), 7)) {
                    Ret.Active = true;
                    break;
                }
            }
            if(!Ret.Active) {
                // if these 2 ports are nonzero, this drive is not ATA compliant
                if(inb(IO_Base+4) && inb(IO_Base+5)) {
                    Ret.Active = false;
                    return Ret;
                }
            }
            // poll
            while(true) {
                // wait for DRQ or ERR to set
                unsigned char status = inb(IO_Base+7);
                if(GetBit(status, 3) || GetBit(status, 0)) {
                    Ret.Active = true;
                    break;
                }
            }
        readvals:
            uint16_t* iden;
            // read the values
            for(int i = 0; i < 256; i++) {
                iden[i] = inw(IO_Base+0); // reads one value
                
            } 
            NS_DELAY(); // give delay to allow for DRQ to reset
            Ret.identify_return = iden;
            Ret = ParseIdentify(Ret);
            FlushCache();

            return Ret;
        }
    }
    void NS_DELAY() {
        byte current = inb(CTRL_Base+0);
        for(int i = 0; i < 13; i++) {
            if(inb(CTRL_Base+0) != current) 
                return;
        }
    }
    Drive ParseIdentify (Drive toParse) {
        toParse.Supports48 = GetBit(toParse.identify_return[83], 10);
        uint32_t max28 = (toParse.identify_return[61] >> 16) | toParse.identify_return[60];

        uint32_t max48_lo = (toParse.identify_return[101] >> 16) | toParse.identify_return[100];
        uint32_t max48_hi = (toParse.identify_return[103] >> 16) | toParse.identify_return[102];
        uint64_t max48 = (max48_hi >> 32) | max48_lo;

        if(!max28) 
            toParse.Supports28 = false;
        else {
            toParse.Supports28 = true;
            toParse.Max28 = max28;
        }
        if(toParse.Supports48) 
            toParse.Max48 = max48;
        
        uint8_t udmaSupported = (uint8_t)toParse.identify_return[88];
        uint8_t udmaActive = (toParse.identify_return[88] << 8);
        
        // get UDMA for drive
        for(int i = 0; i < 8; i++) {
            if(GetBit(udmaActive, i))
                toParse.CurrentUDMA = i;
            if(GetBit(udmaSupported, i))
                toParse.MaxUDMA = i;    
        }
    }
    // clears all the LBA ports
    void ClearLBA() {
        outb(IO_Base+2, 0);
        outb(IO_Base+3, 0);
        outb(IO_Base+4, 0);
        outb(IO_Base+5, 0);
    }
    // selects a drive and returns the subsequent status register
    uint8_t SelectDrive(unsigned char DriveID) {
        uint8_t currentStatus = inb(CTRL_Base+0);
        outb(IO_Base+6, DriveID);

        // poll if value does NOT change in status reg
        uint8_t updated = inb(CTRL_Base+0);
        if(updated != currentStatus) {
            return updated; // return new status
        } else {
            // waste 400ns for the thing to get its shit together
            uint8_t Last = currentStatus; // read once

            for(int i = 0; i < 12; i++) {
                uint8_t current = inb(CTRL_Base+0);
                // if this read is different, you can return it
                if(current != Last) 
                    return current;
                else // otherwise, set last and continue
                    current = Last;
            } 
            return inb(CTRL_Base+0); // return last value
        }
    }
    // writes to a 28 bit LBA
    uint16_t* Read28(uint32_t LBA, Drive drive, int SectorCount=1){
        drive.SetSlavebit(); // make sure slavebit is accurate
        if(drive.DType == Drive::Master) 
            outb(IO_Base+6, 0xE0 |  (drive.slavebit << 4) | ((LBA >> 24) & 0x0F));
        else 
            outb(IO_Base+6, 0xF0 |  (drive.slavebit << 4) | ((LBA >> 24) & 0x0F));
        outb(IO_Base+1, 0x00); // waste a tiny bit of cpu time
        outb(IO_Base+2, (unsigned char)SectorCount); // send scount
        outb(IO_Base+3, (unsigned char)LBA); // low 8 bits
        outb(IO_Base+4, (unsigned char)(LBA >> 8)); // mid 8 bits
        outb(IO_Base+5, (unsigned char)(LBA >> 16)); // high 8 bits

        unsigned char current_status = inb(CTRL_Base+0); // get the status
        // now to read
        outb(IO_Base+7, 0x20); // READ SECTORS command

        while(current_status == inb(CTRL_Base+0)); // poll
        // now actually read all uint16_t's
        uint16_t* ret;
        for(int i = 0; i < 256*SectorCount; i++) {
            ret[i] = inw(IO_Base+0);
            
            // if its about to hit a new sector, flush n' delay
            if(!(i % 256) && i) {
                FlushCache(); // prevent bad sectors
                NS_DELAY(); // make sure to flush out old status
            }
        } NS_DELAY(); // give delay for resetting DRQ/BSY
        return ret; // return the data buffer
    }
    enum ATA_CTRL {
        Primary_C = 0x3F6,
        Secondary_C = 0x376,
        Tertiary_C = 0x3E6,
        Quaternary_C = 0x366
    };
    enum ATA_IO {
        Primary_IO = 0x1F0,
        Secondary_IO = 0x170,
        Tertiary_IO = 0x1E8,
        Quaternary_IO = 0x168
    };
    unsigned char IO_Base = 0x00;
    unsigned char CTRL_Base = 0x00;

};