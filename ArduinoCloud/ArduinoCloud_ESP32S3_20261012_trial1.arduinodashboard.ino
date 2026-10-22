#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_NeoPixel.h>
#include "thingProperties.h"

// Force native USB CDC output for Waveshare ESP32-S3 Zero
#if CONFIG_IDF_TARGET_ESP32S3
#include "HWCDC.h"
HWCDC USBSerial;
#define Serial USBSerial
#endif

// Pin Definitions for Waveshare ESP32-S3 Zero
#define SDA_PIN 1
#define SCL_PIN 2
#define RGB_LED_PIN 21   // Onboard RGB LED pin
#define NUM_PIXELS 1

Adafruit_AHTX0 aht;
Adafruit_NeoPixel rgbLed(NUM_PIXELS, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

unsigned long lastUpdate = 0;
bool ledState = false;

void setup() {
  // Clear any hung Wi-Fi connections before initializing
  WiFi.disconnect(true);
  delay(500);

  Serial.begin(115200);
  
  // Pause briefly for macOS USB CDC connection
  unsigned long start = millis();
  while (!Serial && (millis() - start < 4000)) {
    delay(10);
  }
  
  Serial.println("\n--- ESP32-S3 BOOTING ---");

  // Initialize Onboard RGB LED
  rgbLed.begin();
  rgbLed.setBrightness(30); // 0-255 brightness level
  rgbLed.setPixelColor(0, rgbLed.Color(0, 0, 255)); // Blue during setup
  rgbLed.show();

  // Initialize Arduino IoT Cloud
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // Initialize I2C Bus and AHT Sensor
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!aht.begin()) {
    Serial.println("Could not find AHT10/AHT20 sensor!");
  } else {
    Serial.println("AHT10/AHT20 sensor found!");
  }
}

void loop() {
  ArduinoCloud.update();

  // Read sensor and evaluate LED status every 2 seconds
  if (millis() - lastUpdate > 2000) {
    lastUpdate = millis();

    sensors_event_t humidity_event, temp_event;
    if (aht.getEvent(&humidity_event, &temp_event)) {
      float currentTemp = temp_event.temperature;
      float currentHum = humidity_event.relative_humidity;

      // Print to Serial Monitor
      Serial.print("Temp: ");
      Serial.print(currentTemp);
      Serial.print(" °C | Humidity: ");
      Serial.print(currentHum);
      Serial.println(" %");

      // Push to Cloud Variables
      temperature = currentTemp;
      humidity = currentHum;

      // LED Logic & Cloud Status Update
      if (currentTemp > 25.0) {
        // Flash Red if over 25°C
        ledState = !ledState;
        if (ledState) {
          rgbLed.setPixelColor(0, rgbLed.Color(255, 0, 0)); // Red ON
        } else {
          rgbLed.setPixelColor(0, rgbLed.Color(0, 0, 0));   // OFF
        }
        led_status = "ALERT: High Temp (> 25°C)";
        Serial.println("ALERT: Temp > 25°C! Flashing Red LED.");
      } else {
        // Solid Green if normal
        rgbLed.setPixelColor(0, rgbLed.Color(0, 255, 0));   // Green ON
        led_status = "Normal (LED Green)";
      }
      rgbLed.show();
    }
  }
}
