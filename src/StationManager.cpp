// StationManager.cpp
#include "StationManager.h"

StationManager::StationManager(int leeway)
{
    stationCount = 0;
    this->leeway = leeway;
}

void StationManager::addStation(const String &name, int frequency)
{
    if (stationCount < 10)
    {
        stations[stationCount].name = name;
        stations[stationCount].frequency = frequency;
        stationCount++;
    }
}

int StationManager::getSignalStrength(int potValue, const Station &station) const
{
    int difference = abs(potValue - station.frequency);
    if (difference <= leeway)
    {
        return map(difference, 0, leeway, 255, 0); // Stronger signal as difference approaches 0
    }
    else
    {
        return 0; // No signal outside of leeway
    }
}

const Station *StationManager::getStrongestStation(int potValue, int &signalStrength) const
{
    const Station *strongestStation = nullptr;
    int maxStrength = 0;
    for (int i = 0; i < stationCount; i++)
    {
        int strength = getSignalStrength(potValue, stations[i]);
        if (strength > maxStrength)
        {
            maxStrength = strength;
            strongestStation = &stations[i];
        }
    }
    signalStrength = maxStrength;
    return strongestStation;
}

int StationManager::getOverallSignalStrength(int potValue) const
{
    int overallStrength = 0;
    for (int i = 0; i < stationCount; i++)
    {
        int strength = getSignalStrength(potValue, stations[i]);
        if (strength > overallStrength)
        {
            overallStrength = strength;
        }
    }
    return overallStrength;
}
