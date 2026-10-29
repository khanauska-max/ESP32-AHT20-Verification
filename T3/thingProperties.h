#ifndef THING_PROPERTIES_H
#define THING_PROPERTIES_H

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "arduino_secrets.h"

const char SSID[] = SECRET_SSID;
const char PASS[] = SECRET_OPTIONAL_PASS;

// Declare Cloud Variables as externs
extern float temperature;
extern float cfuPopulation;
extern int batteryPercent;
extern String milkStatus;
extern bool isSystemActive;
extern String cloudLog;

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

void initProperties() {
  ArduinoCloud.setBoardId(BOARD_ID);
  ArduinoCloud.setSecretDeviceKey(SECRET_DEVICE_KEY);

  ArduinoCloud.addProperty(temperature, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(cfuPopulation, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(batteryPercent, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(milkStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(isSystemActive, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(cloudLog, READ, ON_CHANGE, NULL);
}

#endif
