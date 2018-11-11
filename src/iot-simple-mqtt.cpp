#include "iot-simple-mqtt.h"
#include "Arduino.h"
#include <WiFi.h>
#include <PubSubClient.h>

void bmw12::Mqtt::connect(const char *host, const int port, const char *name, const char *username, const char *password)
{
    this->name = name;
    this->username = username;
    this->password = password;

    this->wifiClient = wifiClient;
    this->pubSubClient = PubSubClient(this->wifiClient);

    pubSubClient.setServer(host, port);

    this->check();
}

void bmw12::Mqtt::check()
{
    while (!pubSubClient.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (pubSubClient.connect(this->name, this->username, this->password))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(pubSubClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void bmw12::Mqtt::send(const char *topic, const char *message) {
    this->pubSubClient.publish(topic, message);
}