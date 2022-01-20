#pragma once
#include "Core/Thread.h"

class PIT {
public:
    // called each IRQ0 call
    static void OnFire() {
        ThreadMan.PIT_Fire();
        PIC::SendEOI(0);
        PIC::IRQ_clear_mask(0); // make sure this keeps getting called
    }
};