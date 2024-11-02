#include "Station.h"

Station::Station(const String &name, int frequency, const String &message)
    : name(name), frequency(frequency), message(message)
{
}

const String &Station::getName() const
{
    return name;
}

int Station::getFrequency() const
{
    return frequency;
}

const String &Station::getMessage() const
{
    return message;
}

void Station::setName(const String &name)
{
    this->name = name;
}

void Station::setFrequency(int frequency)
{
    this->frequency = frequency;
}

void Station::setMessage(const String &message)
{
    this->message = message;
}
