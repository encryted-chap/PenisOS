#pragma once
#include "Core/Types.hpp"

class Application {
    unsigned char *instructions;
    int inslen, index;

    Application(unsigned char *ins, int len) {
        inslen = len;
        instructions = ins;
    }
    void Step() {
        if(index == inslen) {
            // kill proc then return
            return;
        } 
    }
    
};