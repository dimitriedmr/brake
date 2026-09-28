/*
based on:
https://github.com/olkal/HX711_ADC/blob/master/examples/Read_1x_load_cell/Read_1x_load_cell.ino

*/

#include <Arduino.h>
#include <Wire.h>
#include "HX711_ADC.h"
#include "Brake.h"

#define DEBUG 1 // 0 = HID only, 1 = HID with serial output

// pins:
#define LED 30
const int HX711_dout = 2; // mcu > HX711 dout pin
const int HX711_sck = 3;  // mcu > HX711 sck pin

// HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);
unsigned long t = 0;

Brake b; // Init USB HID emulation

bool led = false;
void toggleLED()
{
  led = !led;
  digitalWrite(LED, led);
}

// function runs once when you press reset or power the board
void setup(void)
{
#if DEBUG
  Serial.begin(57600);
  delay(10);
  Serial.println("USB HID brake starting...");
  Serial.println();
#endif

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED, OUTPUT);

  LoadCell.begin();
  // LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive

  // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  // set this to false if you don't want tare to be performed in the next step
  LoadCell.start(2000, true);
  if (LoadCell.getTareTimeoutFlag())
  {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1)
      ;
  }
  else
  {
    LoadCell.setCalFactor(696.0); // set calibration value (float)
    Serial.println("Startup complete");
  }
}

// function runs over and over again forever
void loop(void)
{
  uint32_t raw_value = 0;
  static uint32_t l = 0;

// #if DEBUG
//   Serial.print("Loop ");
//   Serial.println(l++);
// #endif
  toggleLED(); // show mcu is running

  // check for new data/start next conversion:
  // get smoothed value from the dataset:
  if (LoadCell.update())
  {
    float raw_value = LoadCell.getData() * 1000;
    // send value
    b.update((uint32_t)raw_value);
    b.usb_update();
#if DEBUG
    Serial.print("Load cell raw value: ");
    Serial.print(raw_value);
    Serial.print(" usb value: ");
    Serial.println(b.usb_brake_value);
#endif
  }
  delay(100); // 100Hz
}
