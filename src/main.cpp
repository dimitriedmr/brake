

#include "Arduino_FreeRTOS.h"
#include "Brake.h"
#include "HX711_ADC.h"
#include "task.h"
// #include <Wire.h> // delay

// up to 4 tasks
static void TaskBlink(void *pvParameters);
static void TaskAnalogRead(void *pvParameters);
static void TaskTestUSB(void *pvParameters);

// function runs once when you press reset or power the board
void setup(void)
{
  // init serial
  Serial.begin(9600); // 57600
  delay(10);          // wait for serial port to connect. Needed for native USB on 32u4 based boards.
  Serial.println("Serial started.");
  Serial.println();

  // Set up tasks.
  // Stack size can be checked & adjusted by reading the Stack Highwater
  // Priority, config MAX_PRIORITIES - 1 = 3 being the highest, and 0 being the lowest.
  xTaskCreate(TaskBlink, "Blink", 128, NULL, 3, NULL); // show that it's running
  xTaskCreate(TaskTestUSB, "TaskTestUSB", 128, NULL, 2, NULL);
  // xTaskCreate(TaskAnalogRead, "AnalogRead", 128, NULL, 1, NULL);
  // ? check Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
  vTaskStartScheduler();
}

static void TaskBlink(void *pvParameters)
{
  /*
   Blink = Turns on an LED on for one second, then off for one second, repeatedly.
   MICRO does not have LED_BUILTIN. Use either LED_BUILTIN_RX or LED_BUILTIN_TX.
   Based on:
   https://registry.platformio.org/libraries/feilipu/FreeRTOS/examples/Blink_AnalogRead/Blink_AnalogRead.ino
 */
  (void)pvParameters;
  bool led_value = false;

  pinMode(LED_BUILTIN_TX, OUTPUT);

  while (1)
  {
    led_value = !led_value; // toggle LED on/off
    digitalWrite(LED_BUILTIN_TX, led_value);
    vTaskDelay(1000 / portTICK_PERIOD_MS); // wait ms to ticks
  }
}

uint32_t raw_value_test = 0;
uint32_t step = 15;
uint32_t loop_no = 0;

void TaskTestUSB(void *pvParameters)
{
  (void)pvParameters;

  Brake brake;
  brake.begin();

  for (;;)
  {
    // check for new data/start next conversion:
    // get smoothed value from the dataset:
    raw_value_test += step;
    raw_value_test %= 255;
    // print out the value you read:
    brake.update(raw_value_test);
    Serial.print(raw_value_test);
    vTaskDelay(1000 / portTICK_PERIOD_MS); // wait for 1000ms
    Serial.print("Loop ");
    Serial.println(loop_no++);
  }
}

void TaskAnalogRead(void *pvParameters)
{
  (void)pvParameters;

  /*
    AnalogReadSerial
    Reads an analog input on pin 0, prints the result to the serial monitor.
    Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
    Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

    This example code is in the public domain.
  */

  const int HX711_dout = 2; // mcu > HX711 dout pin
  const int HX711_sck = 3;  // mcu > HX711 sck pin
  HX711_ADC LoadCell(HX711_dout, HX711_sck);
  LoadCell.begin();
  LoadCell.setReverseOutput(); // uncomment to turn a negative output value to positive
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

  for (;;)
  {
    // read the input on analog pin 0:
    float sensorValue = .0; // = analogRead(A0);
    if (LoadCell.update())
    {
      sensorValue = LoadCell.getData() * 1000;
      Serial.print("Load cell raw value: ");
      Serial.print(sensorValue);
    }
    vTaskDelay(1); // one tick delay (15ms) in between reads for stability
  }
}

// function runs over and over again forever
void loop()
{
  // Empty. Things are done in Tasks.
}
