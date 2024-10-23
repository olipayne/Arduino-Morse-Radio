// PowerManager.h
#ifndef POWERMANAGER_H
#define POWERMANAGER_H

#include <Arduino.h>
#include <UMS3.h>

class PowerManager
{
public:
    PowerManager(UMS3 &ums3, unsigned long sleepTimeout);

    void checkBattery();
    void updateInactivity(bool activityDetected);
    bool shouldSleep() const;
    void resetInactivityTimer();

private:
    UMS3 &ums3;
    unsigned long sleepTimeout;
    unsigned long lastActivityTime;
    unsigned long lastBatteryCheck;
};

#endif
