#include <Arduino.h>
#include "DHT.h"
#include "iot-json-creator.h"
#include "iot-simple-wifi.h"
#include "iot-simple-mqtt.h"
#include "iot-reed-switch.h"
#include <../include/system-config.h>

#define DHTTYPE DHT11

#define TRUE_STR "true"
#define FALSE_STR "false"

bmw12::Wifi wifi;
bmw12::Mqtt mqtt;
DHT dht(DHTPIN, DHTTYPE);

bool reedSwitchState = false;

unsigned long previousSend = 0;
unsigned long previousSendChange = 0;
static const unsigned long sendInterval = 5 * 60 * 1000;
static const unsigned long checkChangeInterval = 500;

void sendCurrentReedState(bool isChangeEvt);
void sendCurrentTempAndHumidity();
void connect();
static const char *BoolToString(bool b);

void setup()
{
  Serial.begin(9600);
  delay(10);

  dht.begin();

  wifi.connect(WIFI_SSID, WIFI_PASSWORD);
  mqtt.connect(MQTT_HOST, MQTT_PORT, "garage", MQTT_USER, MQTT_PASSWORD);

  String *initialMessage = bmw12::createJson("boot", PLACE, "garage-door");
  mqtt.send("iot/" PLACE "/garage-door/reed-switch", initialMessage);
  delete initialMessage;

  sendCurrentReedState(false);
  sendCurrentTempAndHumidity();
  previousSend = millis();
}

void loop()
{
  connect();

  if (millis() - previousSend > sendInterval)
  {
    previousSend = millis();
    sendCurrentReedState(false);
    sendCurrentTempAndHumidity();
  }

  if (millis() - previousSendChange > checkChangeInterval)
  {
    previousSendChange = millis();
    if (bmw12::reedSwitchGet(REED_SWITCH_PIN) != reedSwitchState)
    {

      sendCurrentReedState(true);
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

void sendCurrentReedState(bool isChangeEvt)
{
  reedSwitchState = bmw12::reedSwitchGet(REED_SWITCH_PIN);

  const String *content = bmw12::createJson("reed-switch", PLACE, "garage-door", "garage-door", reedSwitchState, isChangeEvt);

  Serial.printf("current state: %s \n", BoolToString(reedSwitchState));

  mqtt.send("iot/" PLACE "/garage-door/reed-switch/garage-door", content);

  delete content;
}

void sendCurrentTempAndHumidity()
{
  const int maxRetry = 2;

  for (int i = 0; i < maxRetry; i++)
  {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (!isnan(t) && !isnan(h))
    {
      const String *contentTemp = bmw12::createJson("temparature", PLACE, "garage-door", "dht11", t);
      Serial.printf("current temparature: %f \n", t);
      mqtt.send("iot/" PLACE "/garage-door/temparature/dht11", contentTemp);
      delete contentTemp;

      const String *contentHumi = bmw12::createJson("humidity", PLACE, "garage-door", "dht11", h);
      Serial.printf("current humidity: %f \n", h);
      mqtt.send("iot/" PLACE "/garage-door/humidity/dht11", contentHumi);
      delete contentHumi;

      return;
    }
  }

  Serial.println("failed to reed temp and humidity");
}

void connect()
{
  wifi.check();
  mqtt.check();
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