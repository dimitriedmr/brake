/*
  Brake.cpp

  resources:

   https://www.usb.org/document-library/hid-usage-tables-17
   https://www.usb.org/sites/default/files/hut1_7.pdf
   https://eleccelerator.com/usbdescreqparser/#

   Mouse.h (Arduino lib) https://github.com/arduino-libraries/Mouse/tree/master

   https://github.com/MHeironimus/ArduinoJoystickLibrary

   02 SimulationControlsPage
   C5sage Name: Brake  chapter 5.3 Automobile Simulation Devices
  sage Type: DV Dynamic Value; flags: Data, Variable, Absolute; A read/write multiple-bit value.
   Description: A device for slowing or stopping motion, as of a vehicle, especially by contact friction.
                A Brake can be an On/Off Control or a dimensionless single degree-of-freedom dynamic value,
              where the range of values is from zero to maximum braking.

  Based on joystick lib.
  */

#include "Brake.h"

#if defined(_USING_DYNAMIC_HID)

/* Public Interface - May be used in end-application: from LUFA HIDReportData.h */
/* Macros: */
/** \name HID Input, Output and Feature Report Descriptor Item Flags */

#define HID_IOF_CONSTANT (1 << 0)
#define HID_IOF_DATA (0 << 0)
#define HID_IOF_VARIABLE (1 << 1)
#define HID_IOF_ARRAY (0 << 1)
#define HID_IOF_RELATIVE (1 << 2)
#define HID_IOF_ABSOLUTE (0 << 2)
#define HID_IOF_WRAP (1 << 3)
#define HID_IOF_NO_WRAP (0 << 3)
#define HID_IOF_NON_LINEAR (1 << 4)
#define HID_IOF_LINEAR (0 << 4)
#define HID_IOF_NO_PREFERRED_STATE (1 << 5)
#define HID_IOF_PREFERRED_STATE (0 << 5)
#define HID_IOF_NULLSTATE (1 << 6)
#define HID_IOF_NO_NULL_POSITION (0 << 6)
#define HID_IOF_VOLATILE (1 << 7)
#define HID_IOF_NON_VOLATILE (0 << 7)
#define HID_IOF_BUFFERED_BYTES (1 << 8)
#define HID_IOF_BITFIELD (0 << 8)

/*
Alternative 1 - nok
  0x05, 0x01,       // Generic Desktop
  0x09, 0x04,       // Joystick
  0xA1, 0x01,       // Application
  /* The Handbrake Axis (mapped to Z)
  0x09, 0x32,         /*sage Z
  0x15, 0,            /* 0%
  0x26, 0xFF, 0x00,   /* 100%
  0x75, 0x08,         /* 8-bit resolution
  0x95, 0x01,         /* Only 1 axis
  0x81,HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE,
  0xC0, 0,
Alternative 2 - nok
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
Alternative 3 - ok
  0x05, 0x01,        // usage page: (Generic Desktop Ctrls)
  0x09, 0x04,        // USAGE (Joystick)
  0xA1, 0x01,        // COLLECTION (Application)
  0x85, 0x03,        //   REPORT_ID
  0x05, 0x01,        //   USAGE_PAGE (Generic Desktop Ctrls)
  0x09, 0x01,        //   USAGE (Pointer)
  0x15, 0x00,        //   LOGICAL_MINIMUM (0)
  0x26, 0xFF, 0x00,  //   LOGICAL_MAXIMUM (255) / 0xFF, 0xFF LOGICAL_MAXIMUM (65535)
  0x75, 0x08,        //   REPORT_SIZE (8) /  0x10 REPORT_SIZE (16)
  0x95, 0x01,        //   REPORT_COUNT (1)
  0xA1, 0x00,        //   COLLECTION (Phy)
  0x09, 0x30,        //     USAGE (X axis)
  0x81, 0x02,        //     INPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0xC0               //   END_COLLECTION
  0xC0               // END_COLLECTION
*/
#define REPORT_ID 0x03U

static const uint8_t report_descriptor[] = {
    /*USAGE_PAGE Generic Desktop*/ 0x05,
    0x01,
    /*USAGE Joystick*/ 0x09,
    0x04,
    /*COLLECTION Application*/ 0xA1,
    0x01,
    /*REPORT_ID default:3*/ 0x85,
    REPORT_ID,
    /*USAGE_PAGE Generic Desktop*/ 0x05,
    0x01,
    /*USAGE (Pointer)*/ 0x09,
    0x01,
    /*LOGICAL_MINIMUM (0)*/ 0x15,
    0x00,
    /*LOGICAL_MAXIMUM (65535)*/ 0x26,
    0xFF,
    0x00,
    /* REPORT_SIZE (16) */ 0x75,
    0x08,
    /* REPORT_COUNT (axisCount) */ 0x95,
    0x01,
    /* COLLECTION (Physical) */ 0xA1,
    0x00,
    /*USAGE (X axis) */ 0x09,
    0x30,
    /* INPUT (Data,Var,Abs) ... */ 0x81,
    HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE,
    /* END COLLECTION (Physical) */ 0xc0,
    /* END COLLECTION (Physical) */ 0xc0,

};

void Brake::begin()
{
  /* Register HID Report Description */
  DynamicHIDSubDescriptor *node = new DynamicHIDSubDescriptor(report_descriptor, sizeof(report_descriptor), false);
  DynamicHID().AppendDescriptor(node);
}

void Brake::update(int32_t value)
{
  usb_brake_value = value;
  sendState();
}

void Brake::sendState() { DynamicHID().SendReport(REPORT_ID, &usb_brake_value, 1); }

#endif
