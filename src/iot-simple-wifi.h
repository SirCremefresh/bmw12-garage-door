#ifndef iotSimpleWifi_h
#define iotSimpleWifi_h

namespace bmw12
{
class Wifi
{
  public:
    const char *ssid;
    const char *password;

  public:
    void connect(const char *ssid, const char *password);
    void check(bool forceReconnect = false);
};
} // namespace bmw12

#endif