#ifndef STATION_H
#define STATION_H

#include <Arduino.h>

class Station
{
public:
  Station(const String &name, int frequency, const String &message);

  const String &getName() const;
  int getFrequency() const;
  const String &getMessage() const;

  void setName(const String &name);
  void setFrequency(int frequency);
  void setMessage(const String &message);

private:
  String name;
  int frequency;
  String message;
};

#endif // STATION_H
