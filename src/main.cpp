#include <Arduino.h>
#include "iot-json-creator.h"
#include "iot-simple-wifi.h"
#include "iot-simple-mqtt.h"
#include "iot-reed-switch.h"
#include <../include/system-config.h>

#define TRUE_STR "true"
#define FALSE_STR "false"

bmw12::Wifi wifi;
bmw12::Mqtt mqtt;

bool reedSwitchState = false;

unsigned long previousSend = 0;
unsigned long previousSendChange = 0;
static const unsigned long sendInterval = 5 * 60 * 1000;
static const unsigned long checkChangeInterval = 500;

void sendCurrentState(bool isChangeEvt);
static const char *BoolToString(bool b);

void setup()
{
  Serial.begin(9600);
  delay(10);

  wifi.connect(WIFI_SSID, WIFI_PASSWORD);
  mqtt.connect(MQTT_HOST, MQTT_PORT, "garage", MQTT_USER, MQTT_PASSWORD);

  String *initialMessage = bmw12::createJson("boot", PLACE, "garage-door");
  mqtt.send("iot/" PLACE "/garage-door/reed-switch", initialMessage);
  delete initialMessage;

  sendCurrentState(false);
  previousSend = millis();
}

void loop()
{
  wifi.check();
  mqtt.check();

  if (millis() - previousSend > sendInterval)
  {
    previousSend = millis();
    sendCurrentState(false);
  }

  if (millis() - previousSendChange > checkChangeInterval)
  {
    previousSendChange = millis();
    if (bmw12::reedSwitchGet(REED_SWITCH_PIN) != reedSwitchState)
    {
      sendCurrentState(true);
    }
  }

  delay(50);

  // After 50 days milis turns back to 0
  if (millis() < previousSend)
  {
    previousSendChange = millis();
    previousSend = millis();
  }
}

void sendCurrentState(bool isChangeEvt)
{
  reedSwitchState = bmw12::reedSwitchGet(REED_SWITCH_PIN);

  const String *content = bmw12::createJson("reed-switch", "garage-door", PLACE, "garage-door", reedSwitchState, isChangeEvt);

  Serial.printf("current state: %s \n", BoolToString(reedSwitchState));

  mqtt.send("iot/" PLACE "/garage-door/reed-switch/garage-door", content);

  delete content;
}

static const char *BoolToString(bool b)
{
  if (b)
  {
    return TRUE_STR;
  }
  else
  {
    return FALSE_STR;
  }
}