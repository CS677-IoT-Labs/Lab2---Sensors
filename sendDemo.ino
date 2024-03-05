// constants won't change
#define LIGHT_SENSOR_PIN  15
#define LED_PIN           23

#define BYTE_TO_SEND 0b10101010
#define LED_DELAY 500

#define AMBIENT_HIGH 4000
#define AMBIENT_LOW     0

TaskHandle_t ledTask;
TaskHandle_t ldrTask;

int ambientLight = 0; // avg light in the room, calculated in setup

inline void sendByte(uint8_t data)
{
  for (int i = 7; i >= 0; i--) {
    bool bit = (data >> i) & 1;  // Extract the i-th bit from the byte
    digitalWrite(LED_PIN, bit);
    delay(LED_DELAY);
  }
}

void setup()
{
  for(int i = 0; i < 10; i++)
  {
    ambientLight += analogRead(LIGHT_SENSOR_PIN); // read the value on analog pin
    delay(10);
  }

  ambientLight /= 10;

  xTaskCreatePinnedToCore(
                    ledLoop,   /* Task function. */
                    "Sender",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &ledTask,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 0 */ 
  xTaskCreatePinnedToCore(
                    receiveLoop,   /* Task function. */
                    "Receiver",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &ldrTask,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 1 */ 
  
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT); // set ESP32 pin to output mode

  delay(500);
}

void receiveLoop(void* params)
{
  for (;;)
  {
    Serial.print(AMBIENT_LOW); // To freeze the lower limit
    Serial.print(" ");
    Serial.print(AMBIENT_LOW); // To freeze the upper limit
    Serial.print(" ");
    int analogValue = analogRead(LIGHT_SENSOR_PIN); // read the value on analog pin
    int sensor_value = analogValue - ambientLight;

    Serial.println(sensor_value);
  }
}

void ledLoop(void* pvParameters)
{
  for(;;)
  {
    sendByte(BYTE_TO_SEND);
    // delay(1000);
  }
}

void loop() {
  delay(1500);
}