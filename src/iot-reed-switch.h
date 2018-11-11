#ifndef iotReedSwitch_h
#define iotReedSwitch_h

namespace bmw12
{
    bool reedSwitchGet(const int pin, const int tryCount = 3, const int waitMilis = 10);
} // namespace bmw12

#endif