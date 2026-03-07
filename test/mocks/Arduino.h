#ifndef ARDUINO_H
#define ARDUINO_H

#include <stdint.h>
#include <stddef.h>
#include <string>

typedef bool boolean;
typedef uint8_t byte;

class String {
 public:
  String() = default;
  String(const char* value) : data_(value == nullptr ? "" : value) {}
  String(const std::string& value) : data_(value) {}
  String(char value) : data_(1, value) {}

  String& operator=(const char* value) {
    data_ = (value == nullptr ? "" : value);
    return *this;
  }

  String& operator=(const std::string& value) {
    data_ = value;
    return *this;
  }

  const char* c_str() const { return data_.c_str(); }
  size_t length() const { return data_.length(); }
  bool isEmpty() const { return data_.empty(); }
  void clear() { data_.clear(); }

  char operator[](size_t index) const { return data_[index]; }

  bool operator==(const String& other) const { return data_ == other.data_; }
  bool operator!=(const String& other) const { return data_ != other.data_; }

  operator std::string() const { return data_; }

 private:
  std::string data_;
};

#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define PROGMEM
#define F(x) x
#define pgm_read_ptr(addr) (*(addr))

unsigned long millis();
void delay(unsigned long ms);
int digitalRead(int pin);
void digitalWrite(int pin, int value);
void pinMode(int pin, int mode);
int analogRead(int pin);
int random(int min, int max);
long random(long max);

void ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution);
void ledcAttachPin(int pin, uint8_t channel);
void ledcDetachPin(int pin);
void ledcWrite(uint8_t channel, uint32_t duty);
void ledcWriteTone(uint8_t channel, uint32_t freq);
int ledcRead(uint8_t channel);

inline char toupper(char c) { return (c >= 'a' && c <= 'z') ? (c - 'a' + 'A') : c; }
inline int map(int x, int in_min, int in_max, int out_min, int out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
inline int constrain(int x, int min, int max) { return (x < min) ? min : ((x > max) ? max : x); }
inline int abs(int x) { return (x < 0) ? -x : x; }

#endif  // ARDUINO_H
