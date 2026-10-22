This project monitors temperature and humidity using an AHT20 sensor paired with a Waveshare ESP32-S3-Zero board. Sensor data is transmitted live to Arduino IoT Cloud, while an onboard WS2812 RGB LED provides immediate visual status alerts.

Software & Library Dependencies
Adafruit_AXTX0: Handles I2C communication with ATH20 sensors.
Adafruit_NeoPixel: Controls the onboard WS2812 RGB LED.
ArduinoIoTCloud: Manages MQTT connection and variable syncing with Arduino cloud.


Code Breakdown

1. Board & Pin Definitions

#if CONFIG_IDF_TARGET_ESP32S3
#include "HWCDC.h"
HWCDC USBSerial;
#define Serial USBSerial
#endif

Forces native USB CDC logging so output appears in the Arduino IDE Serial Monitor for ESP32-S3 boards.

2. Initialization

Clears pre-existing Wi-Fi radio configurations to prevent reconnect loops.

Initializes teh NeoPixel LED on GPIO 21 and lights it Blue during setup.

Binds I2C pins and verifies AHT sensor initialization.

Registers properties via initProperties() and authenticates with Arduino Cloud.


3. Execution: loop()

Calls ArduinoCloud.update() on every iteration to keep the device synced with the cloud.

Arduino Cloud Configuration

1. Creat a thing named Georgia (ESP32-S3) in Arduino Cloud
2. add the following variables: temp and LED_status
3. Define DEVICE_LOGIN_NAME (Client ID) and SECRET_DEVICE_KEY in thinkgProperties.h / arduino_secrets.h













