#ifndef SIGNAL_MANAGER_H
#define SIGNAL_MANAGER_H

#include <Arduino.h>
#include "Config.h"

class SignalManager
{
public:
    static SignalManager &getInstance()
    {
        static SignalManager instance;
        return instance;
    }

    void begin();
    void updateLockStatus(bool locked);
    void updateSignalStrength(int strength);

#ifdef DEBUG_SERIAL_OUTPUT
    void debugPrint(bool isLocked, const char *stationName = nullptr, int signalStrength = 0);
#endif

private:
    SignalManager() = default;
    SignalManager(const SignalManager &) = delete;
    SignalManager &operator=(const SignalManager &) = delete;

    bool isStationLocked = false;
    bool previousLockState = false;
    unsigned long lastLockPrintTime = 0;
};

#endif
