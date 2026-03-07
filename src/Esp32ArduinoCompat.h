#ifndef ESP32_ARDUINO_COMPAT_H
#define ESP32_ARDUINO_COMPAT_H

#include <Arduino.h>

#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
void ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution);
void ledcAttachPin(uint8_t pin, uint8_t channel);
void ledcDetachPin(uint8_t pin);
bool ledcWriteCompat(uint8_t channel, uint32_t duty);
uint32_t ledcWriteToneCompat(uint8_t channel, uint32_t freq);
uint32_t ledcReadCompat(uint8_t channel);
void adcAttachPin(uint8_t pin);

#define ledcWrite(channel, duty) ledcWriteCompat((channel), (duty))
#define ledcWriteTone(channel, freq) ledcWriteToneCompat((channel), (freq))
#define ledcRead(channel) ledcReadCompat((channel))
#endif

#endif
