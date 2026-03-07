#include "Esp32ArduinoCompat.h"

#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
#ifdef ledcWrite
#undef ledcWrite
#endif
#ifdef ledcWriteTone
#undef ledcWriteTone
#endif
#ifdef ledcRead
#undef ledcRead
#endif

namespace {
constexpr uint8_t kMaxChannels = 16;
uint32_t channelFreq[kMaxChannels] = {0};
uint8_t channelRes[kMaxChannels] = {0};
bool channelConfigured[kMaxChannels] = {false};
int8_t channelPin[kMaxChannels] = {-1, -1, -1, -1, -1, -1, -1, -1,
                                  -1, -1, -1, -1, -1, -1, -1, -1};
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
    if (channelPin[channel] == static_cast<int8_t>(pin)) {
      return;
    }

    if (channelPin[channel] >= 0) {
      ledcDetach(static_cast<uint8_t>(channelPin[channel]));
      channelPin[channel] = -1;
    }

    if (ledcAttachChannel(pin, channelFreq[channel], channelRes[channel], channel)) {
      channelPin[channel] = static_cast<int8_t>(pin);
    }
    return;
  }

  if (ledcAttach(pin, 5000, 8) && channel < kMaxChannels) {
    channelPin[channel] = static_cast<int8_t>(pin);
  }
}

void ledcDetachPin(uint8_t pin) {
  ledcDetach(pin);
  for (size_t i = 0; i < kMaxChannels; i++) {
    if (channelPin[i] == static_cast<int8_t>(pin)) {
      channelPin[i] = -1;
    }
  }
}

bool ledcWriteCompat(uint8_t channel, uint32_t duty) {
  if (channel >= kMaxChannels || channelPin[channel] < 0) {
    return false;
  }

  return ledcWrite(static_cast<uint8_t>(channelPin[channel]), duty);
}

uint32_t ledcWriteToneCompat(uint8_t channel, uint32_t freq) {
  if (channel >= kMaxChannels || channelPin[channel] < 0) {
    return 0;
  }

  return ledcWriteTone(static_cast<uint8_t>(channelPin[channel]), freq);
}

uint32_t ledcReadCompat(uint8_t channel) {
  if (channel >= kMaxChannels || channelPin[channel] < 0) {
    return 0;
  }

  return ledcRead(static_cast<uint8_t>(channelPin[channel]));
}

void adcAttachPin(uint8_t pin) { (void)pin; }
#endif
