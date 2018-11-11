#include "iot-reed-switch.h"
#include "Arduino.h"

bool bmw12::reedSwitchGet(const int pin, const int tryCount, const int waitMilis)
{
    int reedState = 0;

    for (int i = 0; i < tryCount; i++)
    {
        reedState += digitalRead(pin) == 0 ? 1 : 0;
        delay(waitMilis);
    }

    return (reedState >= tryCount - 1) ? true : false;
}
