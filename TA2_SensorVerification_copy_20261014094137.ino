#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// --- Pin Definitions ---
#define I2C_SDA_PIN 8
#define I2C_SCL_PIN 9
#define ONE_WIRE_BUS 4

// --- Object Instances ---
Adafruit_AHTX0 aht;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

// --- Signal Processing / Filtering Variables ---
// Exponential Moving Average (EMA) filter coefficient (alpha between 0.0 and 1.0)
// Lower values = smoother data but slower response time
const float ALPHA = 0.2;
float filtered_aht_temp = 0.0;
bool first_reading = true;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); // Wait for Serial Monitor

  Serial.println(F("ESP32-S3 Dual Temperature Verification Script"));
  Serial.println(F("============================================"));

  // Initialize I2C bus on custom/default ESP32-S3 pins
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  // Initialize AHT20 Sensor
  if (!aht.begin()) {
    Serial.println(F("Error: Could not find AHT20 sensor! Check I2C wiring."));
    while (1) delay(10);
  }
  Serial.println(F("AHT20 Sensor initialized successfully."));

  // Initialize DS18B20 Sensor
  ds18b20.begin();
  int deviceCount = ds18b20.getDeviceCount();
  Serial.print(F("Found "));
  Serial.print(deviceCount);
  Serial.println(F(" DS18B20 device(s) on 1-Wire bus."));

  if (deviceCount == 0) {
    Serial.println(F("Error: No DS18B20 sensor found! Check wiring and 4.7k resistor."));
  }

  Serial.println(F("\nTimestamp (ms) | AHT20 Raw (°C) | AHT20 Filtered (°C) | DS18B20 Ref (°C) | Delta (°C)"));
  Serial.println(F("----------------------------------------------------------------------------------"));
}

void loop() {
  sensors_event_t humidity, temp;
  
  // 1. Read Raw AHT20 Temperature Data
  aht.getEvent(&humidity, &temp);
  float raw_aht_temp = temp.temperature;

  // 2. Read Raw DS18B20 Reference Temperature
  ds18b20.requestTemperatures();
  float ref_ds18b20_temp = ds18b20.getTempCByIndex(0);

  // Handle initialization for Software Noise Filter (EMA)
  if (first_reading) {
    filtered_aht_temp = raw_aht_temp;
    first_reading = false;
  } else {
    // Software Low-Pass Signal Processing (EMA Formula)
    filtered_aht_temp = (ALPHA * raw_aht_temp) + ((1.0 - ALPHA) * filtered_aht_temp);
  }

  // 3. Compute Sensor Error Delta (AHT20 vs Reference)
  float delta = filtered_aht_temp - ref_ds18b20_temp;

  // Output formatted tab-separated values for easy CSV copy/paste into your report
  Serial.print(millis());
  Serial.print("\t\t");
  Serial.print(raw_aht_temp, 2);
  Serial.print("\t\t");
  Serial.print(filtered_aht_temp, 2);
  Serial.print("\t\t\t");
  
  if (ref_ds18b20_temp == DEVICE_DISCONNECTED_C) {
    Serial.print("ERROR");
  } else {
    Serial.print(ref_ds18b20_temp, 2);
  }
  
  Serial.print("\t\t\t");
  Serial.println(delta, 2);

  // Sample Rate Timing: 1 Hz (1 sample per second)
  delay(1000);
}
