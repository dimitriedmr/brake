/*
  Joystick.h

  Copyright (c) 2015-2017, Matthew Heironimus

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef JOYSTICK_h
#define JOYSTICK_h

#include "DynamicHID/DynamicHID.h"

#if ARDUINO < 10606
#error The Joystick library requires Arduino IDE 1.6.6 or greater. Please update your IDE.
#endif  // ARDUINO < 10606

#if ARDUINO > 10606
#if !defined(USBCON)
#error The Joystick library can only be used with a USB MCU (e.g. Arduino Leonardo, Arduino Micro, etc.).
#endif  // !defined(USBCON)
#endif  // ARDUINO > 10606

#if !defined(_USING_DYNAMIC_HID)

#warning "Using legacy HID core (non pluggable)"

#else  // !defined(_USING_DYNAMIC_HID)

//================================================================================
//  Joystick (Gamepad)

#define JOYSTICK_DEFAULT_REPORT_ID 0x03  // wireshark filter usb.src[0:3] == "6.3"
#define JOYSTICK_DEFAULT_AXIS_MINIMUM 0
#define JOYSTICK_DEFAULT_AXIS_MAXIMUM 0xFF
#define JOYSTICK_TYPE_JOYSTICK 0x04

class Joystick_
{
  private:
  // Joystick State
  int32_t _xAxis;
  // Joystick Settings
  bool _autoSendState;
  uint8_t _includeAxisFlags;
  uint8_t _hidReportId;
  uint8_t _hidReportSize;

  public:
  Joystick_();

  void begin(bool initAutoSendState = true);
  void end();

  // Set Axis Values
  void setXAxis(int32_t value);

  void sendState();
};

#endif  // !defined(_USING_DYNAMIC_HID)
#endif  // JOYSTICK_h
