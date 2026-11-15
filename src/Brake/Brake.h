/*
  based on Joystick.h
  */

#ifndef BRAKE_h
#define BRAKE_h

#include "DynamicHID.h"

#if !defined(USBCON)
#error The Joystick library can only be used with a USB MCU (e.g. Arduino Leonardo, Arduino Micro, etc.).
#endif

#if !defined(_USING_DYNAMIC_HID)
#warning "Using legacy HID core (non pluggable)"
#endif

struct Brake
{
  int32_t usb_brake_value;
  void begin(void);
  void update(int32_t value);
  void sendState();
};

#endif
