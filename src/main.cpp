

#include "Arduino_FreeRTOS.h"
#include "Brake.h"
#include "HX711_ADC.h"
#include "queue.h"
#include "task.h"
// #include <Wire.h> // for display?

static void TaskAnalogRead(void *pvParameters);
static void TaskBlink(void *pvParameters);
static void TaskUSB(void *pvParameters);
static void TaskTestRead(void *pvParameters);
static void TaskSerial(void *pvParameters);

QueueHandle_t valuesQueue;

void setup(void)
{
  /* function runs once when you press reset or power the board
   */

  valuesQueue = xQueueCreate(10, sizeof(uint32_t));
  // Set up to 4 tasks; 0 = low prio
  xTaskCreate(TaskTestRead, "TaskTestRead", 128, NULL, 3, NULL);
  // xTaskCreate(TaskAnalogRead, "AnalogRead", 128, NULL, 3, NULL);
  xTaskCreate(TaskUSB, "TaskUSB", 128, NULL, 2, NULL);
  xTaskCreate(TaskBlink, "Blink", 128, NULL, 1, NULL); // blinking shows that it's running,
  // xTaskCreate(TaskSerial, "PrintSerial", 128, NULL, 0, NULL); // it cannot be called

  // Now the task scheduler, which takes over control of scheduling individual tasks, it's automatically started.
}

static void TaskBlink(void *pvParameters)
{
  /*
   It blinks an LED on and off, repeatedly.
   MICRO does not have LED_BUILTIN. Use either LED_BUILTIN_RX or LED_BUILTIN_TX.
   Based on:
   https://registry.platformio.org/libraries/feilipu/FreeRTOS/examples/Blink_AnalogRead/Blink_AnalogRead.ino
 */
  const TickType_t xPeriod = pdMS_TO_TICKS(1000);
  uint8_t led_value = 0x1;

  pinMode(LED_BUILTIN_TX, OUTPUT);

  for (;;)
  {
    led_value = !led_value; // toggle LED on/off
    digitalWrite(LED_BUILTIN_TX, led_value);
    vTaskDelay(xPeriod);
  }
}

void TaskTestRead(void *pvParameters)
{
  /*Task performs a virtual read every x milliseconds. */
  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS(50);

  uint32_t raw_value_test = 0;
  uint32_t step = 10;

  Serial.println("Test start");
  xLastWakeTime = xTaskGetTickCount();
  /* Enter the loop that defines the task behavior. */
  for (;;)
  {
    raw_value_test += step;
    raw_value_test %= 255;
    xQueueSend(valuesQueue, &raw_value_test, portMAX_DELAY);
    vTaskDelayUntil(&xLastWakeTime, xPeriod);
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
  }

  Serial.println("LoadCell started");

  for (;;)
  {
    // read the input on analog pin 0:
    float sensorValue = .0; // = analogRead(A0);
    if (LoadCell.update())
    {
      sensorValue = LoadCell.getData() * 1000;
      xQueueSend(valuesQueue, &sensorValue, portMAX_DELAY);
    }
    vTaskDelay(1); // one tick delay (15ms) in between reads for stability
  }
}

void TaskSerial(void *pvParameters)
{
  /*
   * Prints the received items from the queue to the serial monitor.
   */
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xPeriod = pdMS_TO_TICKS(500);
  uint32_t raw_value_test = 0;

  // Init Arduino serial
  Serial.begin(57600);
  // wait for serial port to connect. Needed for native USB on 32u4 based boards.
  while (!Serial)
  {
    vTaskDelay(1);
  }

  Serial.println("Serial started");

  for (;;)
  {
    if (xQueuePeek(valuesQueue, &raw_value_test, portMAX_DELAY) == pdPASS)
    {
      Serial.println(raw_value_test);
    }
    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}

void TaskUSB(void *pvParameters)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xPeriod = pdMS_TO_TICKS(100);
  uint32_t raw_value_test = 0;

  Brake brake;
  brake.begin();
  Serial.println("USB started");

  for (;;)
  {
    if (xQueueReceive(valuesQueue, &raw_value_test, portMAX_DELAY) == pdPASS)
    {
      brake.update(raw_value_test);
    }
    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}

// function runs over and over again forever
void loop()
{
  // Empty. Things are done in Tasks.
}
