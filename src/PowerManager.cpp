// PowerManager.cpp
#include "PowerManager.h"

PowerManager::PowerManager(UMS3 &ums3, unsigned long sleepTimeout)
    : ums3(ums3), sleepTimeout(sleepTimeout)
{
    lastActivityTime = millis();
    lastBatteryCheck = millis();
}

void PowerManager::checkBattery()
{
    unsigned long currentTime = millis();
    if (currentTime - lastBatteryCheck >= 5000)
    {
        float batteryVoltage = ums3.getBatteryVoltage();
        bool charging = ums3.getVbusPresent();

        // Calculate battery percentage
        float batteryPercentage = ((batteryVoltage - 3.1) / (4.2 - 3.1)) * 100;
        batteryPercentage = constrain(batteryPercentage, 0, 100);

        // Print battery status
        String chargingStatus = charging ? "Yes" : "No";
        Serial.println(String("Battery: ") + batteryPercentage + "% (" + batteryVoltage + "V) Charging: " + chargingStatus);

        lastBatteryCheck = currentTime;
    }
}

void PowerManager::updateInactivity(bool activityDetected)
{
    // Update the last activity time if needed
    if (ums3.getVbusPresent() || activityDetected) // Assuming plugged in or activity detected
    {
        lastActivityTime = millis();
    }
}

bool PowerManager::shouldSleep() const
{
    return (millis() - lastActivityTime) >= sleepTimeout;
}

void PowerManager::resetInactivityTimer()
{
    lastActivityTime = millis();
}
