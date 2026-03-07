#ifndef HARDWARE_EMULATOR_H
#define HARDWARE_EMULATOR_H

#include <array>
#include <cstdint>

class HardwareEmulator {
 public:
  static HardwareEmulator& getInstance();

  void reset();

  void setPinState(int pin, int value);
  void setPinMode(int pin, int mode);
  void incrementDigitalWriteCount(int pin);
  int getPinState(int pin) const;
  int getPinMode(int pin) const;
  int getDigitalWriteCount(int pin) const;

  void setADCValue(int pin, int value);
  int getADCValue(int pin) const;

  void advanceMillis(unsigned long ms);
  unsigned long getMillis() const;

  void setLedc(int channel, uint32_t freq, uint8_t resolution);
  void attachLedcPin(int pin, uint8_t channel);
  void detachLedcPin(int pin);
  void writeLedcDuty(uint8_t channel, uint32_t duty);
  void writeLedcTone(uint8_t channel, uint32_t frequency);

  uint32_t getLedcDuty(uint8_t channel) const;
  uint32_t getLedcTone(uint8_t channel) const;
  uint32_t getLedcFrequency(uint8_t channel) const;
  int getLedcAttachedPin(uint8_t channel) const;
  uint32_t getLedcWriteCount(uint8_t channel) const;

  int nextRandom(int minValue, int maxValueExclusive);
  long nextRandom(long maxValueExclusive);

 private:
  HardwareEmulator() = default;

  static constexpr size_t MAX_PINS = 64;
  static constexpr size_t MAX_CHANNELS = 16;

  struct LedcState {
    uint32_t frequency = 0;
    uint8_t resolution = 0;
    uint32_t duty = 0;
    uint32_t toneFrequency = 0;
    int attachedPin = -1;
    uint32_t writeCount = 0;
  };

  std::array<int, MAX_PINS> pinStates{};
  std::array<int, MAX_PINS> pinModes{};
  std::array<int, MAX_PINS> digitalWriteCounts{};
  std::array<int, MAX_PINS> adcValues{};
  std::array<LedcState, MAX_CHANNELS> ledcStates{};

  unsigned long currentMillis = 0;
  uint32_t randomState = 0x12345678u;
};

#endif
