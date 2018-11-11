#ifndef iotSimpleMqtt_h
#define iotSimpleMqtt_h

#include <WiFi.h>
#include <PubSubClient.h>

namespace bmw12
{
class Mqtt
{
  public:
    const char *name;
    const char *username;
    const char *password;

    WiFiClient wifiClient;
    PubSubClient pubSubClient;

  public:
    void connect(const char *host, const int port, const char *name, const char *username, const char *password);
    void check();
    void send(const char *topic, const char *message);
};
} // namespace bmw12

#endif