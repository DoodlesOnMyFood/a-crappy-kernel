#ifndef __MOUSE_H
#define __MOUSE_H

#include "types.h"
#include "interrupts.h"
#include "port.h"

class MouseDriver : public InterruptHandler
{
    Port8Bit dataport;
    Port8Bit commandport;

    uint8_t buffer[3];
    uint8_t offset;
    uint8_t buttons;
    int8_t x;
    int8_t y;

    public:
        MouseDriver(InterruptManager*);
        ~MouseDriver();
        virtual uint32_t HandleInterrupt(uint32_t);
};

#endif
