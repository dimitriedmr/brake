#include <Arduino.h>

#define LED 30

// the setup function runs once when you press reset or power the board
void setup(void)
{
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED, OUTPUT);
}

// the loop function runs over and over again forever
void loop(void)
{
  digitalWrite(LED, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(5000);             // wait for a second
  digitalWrite(LED, LOW);  // turn the LED off by making the voltage LOW
  delay(1000);             // wait for a second
}
