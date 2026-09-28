/*
  Brake.cpp

  resources:

   https://www.usb.org/document-library/hid-usage-tables-17
   https://www.usb.org/sites/default/files/hut1_7.pdf
   https://eleccelerator.com/usbdescreqparser/#

   Mouse.h (Arduino lib) https://github.com/arduino-libraries/Mouse/tree/master

   https://github.com/MHeironimus/ArduinoJoystickLibrary

   02 SimulationControlsPage
   C5 Usage Name: Brake  chapter 5.3 Automobile Simulation Devices
   Usage Type: DV Dynamic Value; flags: Data, Variable, Absolute; A read/write multiple-bit value.
   Description: A device for slowing or stopping motion, as of a vehicle, especially by contact friction. 
   	            A Brake can be an On/Off Control or a dimensionless single degree-of-freedom dynamic value, 
	            where the range of values is from zero to maximum braking.

*/

#include "Brake.h"

#if defined(_USING_HID)

static const uint8_t _hidReportDescriptor[] PROGMEM = {
  
	0x05, 0x01,        // usage page: (Generic Desktop Ctrls)
	0x09, 0x04,        // USAGE (Joystick)
	0xA1, 0x01,        // COLLECTION (Application)
	0x05, 0x02,        //   USAGE_PAGE (Simulation Controls)
	0x09, 0xC5,        //   USAGE (Brake)
	0x15, 0x00,        //   LOGICAL_MINIMUM (0)
	0x26, 0xFF, 0x00,  //   LOGICAL_MAXIMUM (255) / 0xFF, 0xFF LOGICAL_MAXIMUM (65535)
	0x75, 0x08,        //   REPORT_SIZE (8) /  0x10 REPORT_SIZE (16)
	0x95, 0x01,        //   REPORT_COUNT (1)
	0x81, 0x02,        //   INPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0               // END_COLLECTION

};

Brake::Brake(void)
{
	// create the HID node using the standard Leonardo core implementation
    static HIDSubDescriptor node(_hidReportDescriptor, sizeof(_hidReportDescriptor));
    HID().AppendDescriptor(&node);
}

void Brake::update(uint32_t value)
{
    // scale to 8 bytes (0 - 255) as defined in descriptor
    uint8_t scaled_brake_value = map(value, 0, 0xFFFFFF, 0, 0xFF);
	this->usb_brake_value = scaled_brake_value;
}

void Brake::usb_update(void)
{
	// send data packet
    HID().SendReport(1, &(this->usb_brake_value), sizeof(this->usb_brake_value)); 
}

#endif
