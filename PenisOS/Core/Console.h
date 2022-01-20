#pragma once
#include "Core/HMemory.h"
#include "Core/Types.hpp"
#include "Core/Math.hpp"


unsigned char cfg=7, cbg;
int index;
class console {
public: 
    
    
    enum console_colour {
        black, blue, green, cyan,
        red, magenta, brown, light_gray,
        dark_gray, light_blue, 
        light_green, light_cyan,
        light_red, light_magenta,
        yellow, white
    }; 
    static void set_bg(console_colour col) {
        cbg = (unsigned char)col;
    }
    static void colour_set(console_colour col) {
        cfg = (unsigned char)col;
    }
    static void printcol(char *msg, console_colour col) {
        console_colour lastc = (console_colour)cfg;
        colour_set(col);
        print(msg);
        colour_set(lastc);
    }
    static void clearscreen() {
        index = 0;
        for(int i = 0; i <= (80*25); i++) {
            print(" ");
        }
        index = 0;
    }
    static void printc(char c) {
        char p[2];
        p[0] = c;
        print(p);
    }
    
    static void print(char *msg) {
        if(index >= (80*25)*2 - 25) ScrollDown(); 
        for(int i = 0; msg[i] != '\0'; ++i) {
            switch(msg[i]) {
                case '\n': {
                    while(index % 160) 
                        print(" ");
                } break;
                default: {
                    uint16_t w = 
                        (uint16_t)get_word(
                        msg[i], (console_colour)cbg, (console_colour)cfg
                        );
                    unsigned char *b = (unsigned char*)&w;
                    for(int x = 0; x < 2; x++)
                        *((unsigned char*)0xb8000+index++) = b[x];
                } break;
            }
            
        }
    }
    static void printn(int n) {
        char *buffer;
        int size = 0;
        for(int i = 0; n != 0; i++) {
            char dig = (n % 10) + '0';
            n /= 10; // remove first digit
            buffer[i] = dig;

            size++; // a special tool that will help us later!
        }
        // now i need to reverse *buffer
        for(int i = 0, j = size-1; i < size/2; i++, j--) {
            char temp = buffer[j];
            buffer[j] = buffer[i];
            buffer[i] = buffer[j];
        }
        print(buffer);
    }
    static void ScrollDown() {
        for(int i = 0; i < (80*20)*2; i += 2) {
            ((unsigned char*)0xb8000)[i] = ((unsigned char*)0xb8000)[i+160];
        }
        index -= 80 * 2;
    }
    static void Success(char *msg) {
        console::print("[ ");
        console::colour_set(console::light_green);
        console::print("SUCCESS ");
        console::colour_set(console::light_gray);
        console::print("] ");
        console::print(msg);
        console::print("\n");
    }
private:
    
    static uint16_t get_word(char txt, console_colour bg, console_colour fg) {
        char col = fg | (bg << 4);
        return (uint16_t)((col << 8) | txt);
    }
};