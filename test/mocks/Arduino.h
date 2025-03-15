#ifndef ARDUINO_H
#define ARDUINO_H

#include <stdint.h>
#include <string>

// Define Arduino types
typedef bool boolean;
typedef uint8_t byte;
typedef std::string String;

// Define Arduino constants
#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

// Mock Arduino functions
unsigned long millis();
int digitalRead(int pin);
void digitalWrite(int pin, int value);
void pinMode(int pin, int mode);
int random(int min, int max);

// Mock ESP32 functions
void ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution);
void ledcAttachPin(int pin, uint8_t channel);
void ledcDetachPin(int pin);
void ledcWrite(uint8_t channel, uint32_t duty);
void ledcWriteTone(uint8_t channel, uint32_t freq);
int ledcRead(uint8_t channel);

// String class extensions
namespace Arduino {
inline const char* c_str(const String& s) { return s.c_str(); }
inline bool isEmpty(const String& s) { return s.empty(); }
inline void clear(String& s) { s.clear(); }
inline char charAt(const String& s, size_t index) { return s.at(index); }
inline size_t length(const String& s) { return s.length(); }
}  // namespace Arduino

// Utility functions
inline char toupper(char c) { return (c >= 'a' && c <= 'z') ? (c - 'a' + 'A') : c; }
inline int map(int x, int in_min, int in_max, int out_min, int out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
inline int constrain(int x, int min, int max) { return (x < min) ? min : ((x > max) ? max : x); }
inline int abs(int x) { return (x < 0) ? -x : x; }

#endif  // ARDUINO_H