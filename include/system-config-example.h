
/*

Rename this file to: system-config.h
and fill in your ssid and password

*/


#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include<string>

struct SystemConfig
{
    std::string ssid, password, name, mqtthost, mqttuser, mqttpassword;
    int mqttport;

    SystemConfig(std::string ssid, std::string password, std::string name, std::string mqtthost, int mqttport, std::string mqttuser, std::string mqttpassword)
    {
        this->password = password;
        this->ssid = ssid;
        this->name = name;
        this->mqtthost = mqtthost;
        this->mqttport = mqttport;
        this->mqttuser = mqttuser;
        this->mqttpassword = mqttpassword;
    }
};

SystemConfig systemConfig = SystemConfig("your-ssid", "your-password", "your-name", "mqtt-host", 1234, "mqtt-user", "mqtt-password");

#endif 