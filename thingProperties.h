#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "arduino_secrets.h"

const char DEVICE_LOGIN_NAME[] = "a5a0c1b2-f7fa-4590-ad85-6c7edbc67375";
const char SSID[]              = SECRET_SSID;
const char PASS[]              = SECRET_OPTIONAL_PASS;
const char KEY[]               = SECRET_DEVICE_KEY;

// Declare Cloud Variables
float temperature;
float humidity;
String led_status;  // Cloud status string

void initProperties() {
  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(KEY);

  // Register variables with Arduino Cloud
  ArduinoCloud.addProperty(temperature, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(humidity, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(led_status, READ, ON_CHANGE, NULL);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);