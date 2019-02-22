#pragma once

#include <Arduino.h>

#include "FrequencyCalculator.h"

struct PwmChanger {
    PwmChanger() {
        pinMode(2, OUTPUT);
        analogWrite(2, 1);
    }

    void ApplyRegs(const FrequencyCalculator::RegCountDuty &regs) {
//    CS12;CS11;CS10
//    0    0     1    No Prescaling
//    0    1     0    Clock / 8
//    0    1     1    Clock / 64
//    1    0     0    Clock / 256
//    1    0     1    Clock / 1024
        uint8_t cs = 1;
        if (regs.scaler == 0)
            cs = 1;
        else if (regs.scaler == 8)
            cs = 2;
        else if (regs.scaler == 64)
            cs = 3;
        else if (regs.scaler == 256)
            cs = 4;
        else if (regs.scaler == 1024)
            cs = 5;

        TCCR3B = _BV(WGM33) | _BV(CS01) | _BV(CS00); // 64 divisor
        TCCR3B = _BV(WGM33) | cs; // 8 divisor
        OCR3A = regs.count_reg;
        OCR3B = regs.duty_reg;
    }
};
