


                                    SET UP



.........................................

#include <Wire.h>                       Imports Adruino's built-in I2C communication library, which handles hardware data transfers on the SDA and SCL pins for the AHT20 sensor
#include <Adafruit_AHTX0.h>             Imports Adafruit's driver library for the AHT20 sensor, providing functions to initialize the sensor and read temperature/humidity data over I2C.
#include <OneWire.h>                    Imports the low-level library required to communicate using Maxim/Dallas's proprietary 1-Wire hardware protocol used by the DS18B20 probe.
#include <DallasTemperature.h>          Imports the higher-level driver library that converts raw binary temperature readings from the 1-Wire protocol to clean celcius/farenheit values.

.........................................

#define I2C_SDA_PIN 8                  #define --> Assigns human-readable names to physical GPIO pin numbers to the ESP32-S3
#define I2C_SCL_PIN 9                  
#define ONE_WIRE_BUS 4

.........................................

Adafruit_AHTX0 aht;                   Creates an object named aht that manages the AHT20 sensor.
OneWire oneWire(ONE_WIRE_BUS);        Creates oneWire object and assigns it to operate pin 4.
DallasTemperature ds18b20(&oneWire);  Passes the address of the oneWire object to the DS18B20 manager so it knows which physical line to query for readings.

.........................................

const float ALPHA = 0.2;              Sets the smoothing coefficient for our Exponential Moving Average (EMA) filter. A value of 0.2 means 20% weight is given to the latest incoming reading and 80% to past historical values.
float filtered_aht_temp = 0.0;        A decimal variable storing the running, smoothed temperature output.
bool first_reading = true;            A boolean flag used to properly initialie the filter on the very first loop iteration.

.........................................

void setup() {                        
  Serial.begin(115200);               Opens USB serial communication at 115,200 bits per second.
  while (!Serial) delay(10);          Pauses execution briefly until the USB serial connection is ready.

  .........................................

  Serial.println(F("ESP32-S3 Dual Temperature Verification Script"));
  Serial.println(F("============================================"));

  .........................................

Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); Initializes the I2C hardware bus

  .........................................

if (!aht.begin()) {
    Serial.println(F("Error: Could not find AHT20 sensor! Check I2C wiring."));
    while (1) delay(10);
  }
  Serial.println(F("AHT20 Sensor initialized successfully."));

                                      Sends start up query over to I2C to verify AHT20 is connected and responsive.
                                      If fails, print fails.

.........................................

ds18b20.begin();                      Powers up and initializes the 1-Wire interface.
  int deviceCount = ds18b20.getDeviceCount();
  Serial.print(F("Found "));
  Serial.print(deviceCount);
  Serial.println(F(" DS18B20 device(s) on 1-Wire bus."));

.........................................

if (deviceCount == 0) {
    Serial.println(F("Error: No DS18B20 sensor found! Check wiring and 4.7k resistor."));
  }
                                    Warns if no DS18B20 was detected.

.........................................

Serial.println(F("\nTimestamp (ms) | AHT20 Raw (°C) | AHT20 Filtered (°C) | DS18B20 Ref (°C) | Delta (°C)"));
  Serial.println(F("----------------------------------------------------------------------------------"));
}
                                    Prints header column labels separated by veritcal bar to structure the serial monitor.

.........................................





                                          MAIN LOOP

.........................................

void loop() {
  sensors_event_t humidity, temp;           Declars structures to hold sensor measurement events from Adafruit's API.
  aht.getEvent(&humidity, &temp);           Queries the AHT20 over I2C and fills humidity and temp with raw data.
  float raw_aht_temp = temp.temperature;    Extracts the raw temperature value in C and stores it in raw_aht_temp.

.........................................

ds18b20.requestTemperatures();              Asks probe to perform analog to digital temp conversion.
  float ref_ds18b20_temp = ds18b20.getTempCByIndex(0);

.........................................

if (first_reading) {                       First filtered reading set to equal raw for filter doesn't ramp up slowly.
    filtered_aht_temp = raw_aht_temp;
    first_reading = false;
  } else {
    filtered_aht_temp = (ALPHA * raw_aht_temp) + ((1.0 - ALPHA) * filtered_aht_temp);
  }

.........................................

float delta = filtered_aht_temp - ref_ds18b20_temp;         Subracts reference standard temp from target sensor value.

.........................................

Serial.print(millis());
  Serial.print("\t\t");
  Serial.print(raw_aht_temp, 2);
  Serial.print("\t\t");
  Serial.print(filtered_aht_temp, 2);
  Serial.print("\t\t\t");

.........................................

  if (ref_ds18b20_temp == DEVICE_DISCONNECTED_C) {
    Serial.print("ERROR");
  } else {
    Serial.print(ref_ds18b20_temp, 2);
  }

  delay(1000);
}
.........................................


















