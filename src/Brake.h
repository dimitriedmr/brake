/*
  Brake.h
*/

#ifndef BRAKE_h
#define BRAKE_h

#include "HID.h"

#if !defined(_USING_HID)
#error "It can be used with a USB MCU (e.g. Arduino Leonardo, Micro, etc.)."
#endif

class Brake
{
// private:

public:
    uint8_t usb_brake_value;
    Brake(void);
    void update(uint32_t value);
    void usb_update(void);
};

#endif 
