#include "iot-simple-wifi.h"
#include "Arduino.h"
#include "WiFi.h"

void bmw12::Wifi::connect(const char *ssid, const char *password)
{
    this->ssid = ssid;
    this->password = password;

    this->check(true);
}

void bmw12::Wifi::check(bool forceReconnect)
{
    if (WiFi.status() != WL_CONNECTED || forceReconnect)
    {
        WiFi.disconnect();
        WiFi.mode(WIFI_AP_STA);
        WiFi.begin(this->ssid, this->password);

        Serial.println("Connecting to Wifi");

        while (WiFi.status() != WL_CONNECTED)
        {
            Serial.print(".");
            delay(250);
        }

        Serial.println("");
        Serial.println("WiFi Connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }
}
