/*
  Joystick.cpp

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

#include "Joystick.h"

#if defined(_USING_DYNAMIC_HID)

#define JOYSTICK_REPORT_ID_INDEX 7
#define JOYSTICK_AXIS_MINIMUM 0
#define JOYSTICK_AXIS_MAXIMUM 65535

#define JOYSTICK_INCLUDE_X_AXIS B00000001

// const USB_Descriptor_HIDReport_Datatype_t PROGMEM JoystickReport[] = {
//     HID_RI_USAGE_PAGE(8, 0x01), /* Generic Desktop */
//     HID_RI_USAGE(8, 0x04),      /* Joystick */
//     HID_RI_COLLECTION(8, 0x01), /* Application */

//     /* The Handbrake Axis (mapped to Z) */
//     HID_RI_USAGE(8, 0x32),          /* Usage Z */
//     HID_RI_LOGICAL_MINIMUM(8, 0),   /* 0% */
//     HID_RI_LOGICAL_MAXIMUM(8, 255), /* 100% */
//     HID_RI_REPORT_SIZE(8, 8),       /* 8-bit resolution */
//     HID_RI_REPORT_COUNT(8, 1),      /* Only 1 axis */
//     HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),

//     HID_RI_END_COLLECTION(0),
// };

Joystick_::Joystick_()
{
  // Set the USB HID Report ID
  _hidReportId = JOYSTICK_DEFAULT_REPORT_ID;
  _autoSendState = false;
  _includeAxisFlags = JOYSTICK_INCLUDE_X_AXIS;

  // Build Joystick HID Report Description

  uint8_t tempHidReportDescriptor[150];
  int hidReportDescriptorSize = 0;

  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

  // USAGE (Joystick - 0x04; Gamepad - 0x05; Multi-axis Controller - 0x08)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
  tempHidReportDescriptor[hidReportDescriptorSize++] = JOYSTICK_TYPE_JOYSTICK;

  // COLLECTION (Application)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0xa1;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

  // REPORT_ID (Default: 3)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x85;
  tempHidReportDescriptor[hidReportDescriptorSize++] = _hidReportId;

  // USAGE_PAGE (Generic Desktop)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

  // USAGE (Pointer)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

  // LOGICAL_MINIMUM (0)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x15;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

  // LOGICAL_MAXIMUM (65535)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x26;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0XFF;
  // tempHidReportDescriptor[hidReportDescriptorSize++] = 0XFF;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;
  // tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

  // REPORT_SIZE (16)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x08;

  // REPORT_COUNT (axisCount)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 1;  // axis count

  // COLLECTION (Physical)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0xA1;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

  // USAGE (X)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x30;

  // INPUT (Data,Var,Abs)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

  // END_COLLECTION (Physical)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;

  // END_COLLECTION
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;

  // Create a copy of the HID Report Descriptor template that is just the right size
  uint8_t* customHidReportDescriptor = new uint8_t[hidReportDescriptorSize];
  memcpy(customHidReportDescriptor, tempHidReportDescriptor, hidReportDescriptorSize);

  // Register HID Report Description
  DynamicHIDSubDescriptor* node =
      new DynamicHIDSubDescriptor(customHidReportDescriptor, hidReportDescriptorSize, false);
  DynamicHID().AppendDescriptor(node);

  // Initialize Joystick State
  _xAxis = 0;
}

void Joystick_::begin(bool initAutoSendState)
{
  _autoSendState = initAutoSendState;
  sendState();
}

void Joystick_::end() {}

void Joystick_::setXAxis(int32_t value)
{
  _xAxis = value;
  if (_autoSendState)
    sendState();
}

void Joystick_::sendState() { DynamicHID().SendReport(_hidReportId, &_xAxis, 1); }

#endif
