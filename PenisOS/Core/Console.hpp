#pragma once
#include "Core/Types.hpp"
#include "Core/IO.hpp"

int Index = 0;
int ProtectedIndex;
enum ConsoleColour {
    Black, Blue, Green,
    Cyan, Red, Magenta, Brown, LightGray, Default = 7,
    DarkGray, LightBlue, LightGreen, LightCyan,
    LightRed, LightMagenta, Yellow, White
};
ConsoleColour ForegroundColour = ConsoleColour::Default;
ConsoleColour BackgroundColour = ConsoleColour::Black;

class Console {
public:
    bool shiftmode = false;
    static void Initialize() {
        Clear();
        UpdateCursor();
    }
    // resets the consoles colours back to defaults
    static void ResetCol() {
        ForegroundColour = Default;
        BackgroundColour = Black;
    }
    static void Clear() {
        for(int i = 0; i < (80 * 25) * 2; i += 2)
            ((char*)0xb8000)[i] = '\0';
        Index = 0;
        UpdateCursor();
    }
    static void ScrollOnce() {
        // math that i don't wanna explain :)
        
        for(int i = (80*2); i < (80*25)*2; i++) {
            ((char*)0xb8000)[i-(80*2)] = ((char*)0xb8000)[i];
        }
        int offset = (80*25) * 2;
        for(int i = 0; i < 80; i += 2) {
            ((char*)0xb8000)[i+offset] = ' ';
        }
        Index -= 80*2;
        UpdateCursor();
    }
    // writes a char to the console and advances the cursor
    static void Write(char c, bool prot = true) {
        switch(c) {
            case '\0': return;
            case '\b':
                BackSp();
                break;
            case '\n': {
                if((Index/2) % 80 * 25 == 0) Index += 2;
                for(; (Index/2) % 80 * 25 != 0; Index+=2);
                UpdateCursor();
            } break;
            default:
                ((char*)0xb8000)[Index++] = c;
                ((char*)0xb8000)[Index++] = (BackgroundColour >> 4) | (ForegroundColour & 0xFF);
                break;
        }
        if(prot) {
            ProtectedIndex = Index;
        }
        UpdateCursor();
    }
    // writes an integer to console
    static void Write(int number) {
        bool negative = number < 0;
        if(negative) number = -number;

        if(number < 10) {
            if(!negative)
                Write((char)(number + '0'));
            else {
                char* arr = "- ";
                arr[1] = number + '0';
                Write(arr);
            }
            return;
        }
        char* arr;
        int i = 0;
        while (number > 0) {
            int digit = number % 10;
            number /= 10;
            arr[i++] = digit+'0';
        }
        int realend = i;
        int end = i;
        for(i = 0; i != end; i++, end--) {
            char temp = arr[i];
            arr[i] = arr[end];
            arr[end] = temp;
        }
        arr[realend+1] = '\0';
        for(int i = 0; i < realend+1; i++) {
            arr[i] = arr[i+1];
        }
        if(negative) {
            char* towrite = "-";
            for(int i = 1, arri = 0; arr[arri] != '\0'; i++, arri++) {
                towrite[i] = arr[arri];
                towrite[i+1] = '\0';
            }
            arr = towrite;
        }
        Write(arr); 
    }
    static void BackSp() {
        if(Index == ProtectedIndex) return;
        else {
            ((char*)0xb8000)[--Index] = ForegroundColour;
            ((char*)0xb8000)[--Index] = ' ';
        }
    }
    static void Write(char* str) {
        for(int i = 0; str[i] != '\0'; i++)
            Write(str[i]);
    }
    
    static void SetCursorCol(ConsoleColour col) {
        ((char*)0xb8000)[Index+1] = col;
    }
    static void ResetCursorCol() {
        SetCursorCol(Default);
    }
    static void SuccessMsg(char* msg) {
        Write("[ ");
        ForegroundColour = Green;
        Write("success");
        ResetCol();
        Write(" ] "); Write(msg);
        Write('\n');
    }
    static void FailMsg(char* msg) {
        Write("[ ");
        ForegroundColour = Red;
        Write("failure");
        ResetCol();
        Write(" ] "); Write(msg);
        Write('\n');
    }
    static void KernelMsg(char* msg) {
        Write("[ ");
        ForegroundColour = LightMagenta;
        Write("kernel");
        ResetCol();
        Write(" ] "); Write(msg);
        Write('\n');
    }
    static void UpdateCursor() {
        int pos = 0;

        if(Index == 0) pos = 1;
        else pos = (Index / 2);
        
        if(pos > (80*25)-80) ScrollOnce();
        outb(0x3D4, 0x0F);
        outb(0x3D5, (uint8_t) (pos & 0xFF));
        outb(0x3D4, 0x0E);
        outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
    }
private:
    // shifts all letters by times, doesn't effect before from
    static void ShiftAll(int times, int from = 0) {
        char* buf = (char*)0xb8000;
        buf[(from*2) + 2] = ' ';
        for(; times > 0; times--)
            for(int i = ((80 * 25) * 2); i > from*2; i--) {
                buf[i] = buf[i-2];
            }
    }
    
};