#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "iot-json-creator.h"
#include <../include/system-config.h>

#define TRUE_STR "true"
#define FALSE_STR "false"

WiFiClient espClient;
PubSubClient client(espClient);

const int wifiLedPin = BUILTIN_LED;
const int reedSwitchPin = 13;

bool reedSwitchState;


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

void WIFI_Connect()
{
  WiFi.disconnect();
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    digitalWrite(wifiLedPin, LOW);
    Serial.print(".");
    delay(250);
    digitalWrite(wifiLedPin, HIGH);
  }

  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  digitalWrite(wifiLedPin, HIGH);
}

void MQTT_Connect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(PLACE, MQTT_USER, MQTT_PASSWORD))
    {
      Serial.println("connected");

      client.subscribe(PLACE "-get-state");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

bool getReedSwitchState()
{
  int reedState = 0;
  reedState += digitalRead(reedSwitchPin) == 0 ? 1 : 0;
  delay(10);
  reedState += digitalRead(reedSwitchPin) == 0 ? 1 : 0;
  delay(10);
  reedState += digitalRead(reedSwitchPin) == 0 ? 1 : 0;

  return (reedState >= 2) ? true : false;
}

void sendCurrentState(bool isChangeEvt)
{
  reedSwitchState = getReedSwitchState();

  char *content = bmw12::createJson("reed-switch", PLACE, SENSOR_NAME, reedSwitchState, isChangeEvt);

  Serial.printf("current state: %s", BoolToString(reedSwitchState));

  client.publish("iot/" PLACE "/reed-switch/garage-door", content);

  free(content);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.println("sending current state on request");
  sendCurrentState(false);
}

void setup()
{
  Serial.begin(9600);
  delay(10);

  pinMode(wifiLedPin, OUTPUT);
  WIFI_Connect();

  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(callback);

  MQTT_Connect();

  sendCurrentState(false);
}

unsigned long previousMillis = 0;
long sendInterval = 5 * 60 * 1000;
long checkStateChangeInterval = 500;

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    WIFI_Connect();
  }
  if (!client.connected())
  {
    MQTT_Connect();
  }

  client.loop();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > sendInterval)
  {
    previousMillis = currentMillis;
    sendCurrentState(false);
  }

  if (currentMillis - previousMillis > checkStateChangeInterval)
  {
    previousMillis = currentMillis;
    if (getReedSwitchState() != reedSwitchState)
    {
      sendCurrentState(true);
    }
  }

  delay(50);
}
