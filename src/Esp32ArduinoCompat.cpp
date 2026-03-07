#include "Esp32ArduinoCompat.h"

#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
namespace {
constexpr uint8_t kMaxChannels = 16;
uint32_t channelFreq[kMaxChannels] = {0};
uint8_t channelRes[kMaxChannels] = {0};
bool channelConfigured[kMaxChannels] = {false};
}

void ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution) {
  if (channel < kMaxChannels) {
    channelFreq[channel] = freq;
    channelRes[channel] = resolution;
    channelConfigured[channel] = true;
  }
}

void ledcAttachPin(uint8_t pin, uint8_t channel) {
  if (channel < kMaxChannels && channelConfigured[channel]) {
    ledcAttachChannel(pin, channelFreq[channel], channelRes[channel], channel);
    return;
  }
  ledcAttach(pin, 5000, 8);
}

void ledcDetachPin(uint8_t pin) { ledcDetach(pin); }

void adcAttachPin(uint8_t pin) { (void)pin; }
#endif
