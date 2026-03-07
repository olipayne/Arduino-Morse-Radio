#include "HardwareEmulator.h"

#include "Arduino.h"

HardwareEmulator& HardwareEmulator::getInstance() {
  static HardwareEmulator instance;
  return instance;
}

void HardwareEmulator::reset() {
  pinStates.fill(HIGH);
  pinModes.fill(INPUT);
  digitalWriteCounts.fill(0);
  adcValues.fill(0);
  ledcStates.fill({});
  currentMillis = 0;
  randomState = 0x12345678u;
}

void HardwareEmulator::setPinState(int pin, int value) {
  if (pin >= 0 && pin < static_cast<int>(MAX_PINS)) {
    pinStates[pin] = value;
  }
}

void HardwareEmulator::setPinMode(int pin, int mode) {
  if (pin >= 0 && pin < static_cast<int>(MAX_PINS)) {
    pinModes[pin] = mode;
  }
}

void HardwareEmulator::incrementDigitalWriteCount(int pin) {
  if (pin >= 0 && pin < static_cast<int>(MAX_PINS)) {
    digitalWriteCounts[pin]++;
  }
}

int HardwareEmulator::getPinState(int pin) const {
  if (pin >= 0 && pin < static_cast<int>(MAX_PINS)) {
    return pinStates[pin];
  }
  return HIGH;
}

int HardwareEmulator::getPinMode(int pin) const {
  if (pin >= 0 && pin < static_cast<int>(MAX_PINS)) {
    return pinModes[pin];
  }
  return INPUT;
}

int HardwareEmulator::getDigitalWriteCount(int pin) const {
  if (pin >= 0 && pin < static_cast<int>(MAX_PINS)) {
    return digitalWriteCounts[pin];
  }
  return 0;
}

void HardwareEmulator::setADCValue(int pin, int value) {
  if (pin >= 0 && pin < static_cast<int>(MAX_PINS)) {
    adcValues[pin] = value;
  }
}

int HardwareEmulator::getADCValue(int pin) const {
  if (pin >= 0 && pin < static_cast<int>(MAX_PINS)) {
    return adcValues[pin];
  }
  return 0;
}

void HardwareEmulator::advanceMillis(unsigned long ms) { currentMillis += ms; }

unsigned long HardwareEmulator::getMillis() const { return currentMillis; }

void HardwareEmulator::setLedc(int channel, uint32_t freq, uint8_t resolution) {
  if (channel >= 0 && channel < static_cast<int>(MAX_CHANNELS)) {
    ledcStates[channel].frequency = freq;
    ledcStates[channel].resolution = resolution;
  }
}

void HardwareEmulator::attachLedcPin(int pin, uint8_t channel) {
  if (channel < MAX_CHANNELS) {
    ledcStates[channel].attachedPin = pin;
  }
}

void HardwareEmulator::detachLedcPin(int pin) {
  for (auto& state : ledcStates) {
    if (state.attachedPin == pin) {
      state.attachedPin = -1;
    }
  }
}

void HardwareEmulator::writeLedcDuty(uint8_t channel, uint32_t duty) {
  if (channel < MAX_CHANNELS) {
    ledcStates[channel].duty = duty;
    ledcStates[channel].writeCount++;
  }
}

void HardwareEmulator::writeLedcTone(uint8_t channel, uint32_t frequency) {
  if (channel < MAX_CHANNELS) {
    ledcStates[channel].toneFrequency = frequency;
  }
}

uint32_t HardwareEmulator::getLedcDuty(uint8_t channel) const {
  if (channel < MAX_CHANNELS) {
    return ledcStates[channel].duty;
  }
  return 0;
}

uint32_t HardwareEmulator::getLedcTone(uint8_t channel) const {
  if (channel < MAX_CHANNELS) {
    return ledcStates[channel].toneFrequency;
  }
  return 0;
}

uint32_t HardwareEmulator::getLedcFrequency(uint8_t channel) const {
  if (channel < MAX_CHANNELS) {
    return ledcStates[channel].frequency;
  }
  return 0;
}

int HardwareEmulator::getLedcAttachedPin(uint8_t channel) const {
  if (channel < MAX_CHANNELS) {
    return ledcStates[channel].attachedPin;
  }
  return -1;
}

uint32_t HardwareEmulator::getLedcWriteCount(uint8_t channel) const {
  if (channel < MAX_CHANNELS) {
    return ledcStates[channel].writeCount;
  }
  return 0;
}

int HardwareEmulator::nextRandom(int minValue, int maxValueExclusive) {
  if (maxValueExclusive <= minValue) {
    return minValue;
  }
  randomState = randomState * 1664525u + 1013904223u;
  const int range = maxValueExclusive - minValue;
  return minValue + static_cast<int>(randomState % static_cast<uint32_t>(range));
}

long HardwareEmulator::nextRandom(long maxValueExclusive) {
  if (maxValueExclusive <= 0) {
    return 0;
  }
  randomState = randomState * 1664525u + 1013904223u;
  return static_cast<long>(randomState % static_cast<uint32_t>(maxValueExclusive));
}

unsigned long millis() { return HardwareEmulator::getInstance().getMillis(); }

void delay(unsigned long ms) { HardwareEmulator::getInstance().advanceMillis(ms); }

int digitalRead(int pin) { return HardwareEmulator::getInstance().getPinState(pin); }

void digitalWrite(int pin, int value) {
  auto& hw = HardwareEmulator::getInstance();
  hw.setPinState(pin, value);
  hw.incrementDigitalWriteCount(pin);
}

void pinMode(int pin, int mode) {
  HardwareEmulator::getInstance().setPinMode(pin, mode);
}

int analogRead(int pin) { return HardwareEmulator::getInstance().getADCValue(pin); }

int random(int min, int max) { return HardwareEmulator::getInstance().nextRandom(min, max); }

long random(long max) { return HardwareEmulator::getInstance().nextRandom(max); }

void ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution) {
  HardwareEmulator::getInstance().setLedc(channel, freq, resolution);
}

void ledcAttachPin(int pin, uint8_t channel) {
  HardwareEmulator::getInstance().attachLedcPin(pin, channel);
}

void ledcDetachPin(int pin) { HardwareEmulator::getInstance().detachLedcPin(pin); }

void ledcWrite(uint8_t channel, uint32_t duty) {
  HardwareEmulator::getInstance().writeLedcDuty(channel, duty);
}

void ledcWriteTone(uint8_t channel, uint32_t freq) {
  HardwareEmulator::getInstance().writeLedcTone(channel, freq);
}

int ledcRead(uint8_t channel) { return static_cast<int>(HardwareEmulator::getInstance().getLedcDuty(channel)); }
