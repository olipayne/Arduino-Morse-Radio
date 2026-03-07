#include "AudioManager.h"

#include <vector>

#include "AudioProbe.h"

namespace {
std::vector<AudioEvent> g_events;
}

namespace AudioProbe {
void clear() { g_events.clear(); }
const std::vector<AudioEvent>& events() { return g_events; }
}

void AudioManager::begin() {
  configurePWM();
  currentVolume = static_cast<int>(ConfigManager::getInstance().getSpeakerVolume());
  if (currentVolume < 0) {
    currentVolume = 0;
  }
  if (currentVolume > VOLUME_MAX) {
    currentVolume = VOLUME_MAX;
  }
  isPlayingMorse = false;
  isStaticPlaying = false;
}

void AudioManager::configurePWM() {
  ledcSetup(Audio::SPEAKER_CHANNEL, MORSE_FREQUENCY, 8);
  ledcAttachPin(Pins::SPEAKER, Audio::SPEAKER_CHANNEL);
}

int AudioManager::calculateVolumeLevel(int adcValue) {
  if (adcValue <= VOLUME_DEAD_ZONE) {
    return 0;
  }
  int mapped = map(adcValue, VOLUME_DEAD_ZONE, Radio::ADC_MAX, 0, VOLUME_MAX);
  return constrain(mapped, 0, VOLUME_MAX);
}

int AudioManager::calculateStaticVolume() {
  int staticIntensity = map(currentSignalStrength, 0, 255, 255, 0);
  int staticVolume = map(staticIntensity, 0, 255, 0, currentVolume);
  return constrain(staticVolume, 0, VOLUME_MAX);
}

void AudioManager::setVolume(int adcValue) {
  currentVolume = calculateVolumeLevel(adcValue);
  if (currentVolume == 0) {
    ledcWrite(Audio::SPEAKER_CHANNEL, 0);
    ledcDetachPin(Pins::SPEAKER);
    return;
  }
  ledcAttachPin(Pins::SPEAKER, Audio::SPEAKER_CHANNEL);
  if (isPlayingMorse) {
    ledcWrite(Audio::SPEAKER_CHANNEL, currentVolume);
  } else if (isStaticPlaying) {
    ledcWrite(Audio::SPEAKER_CHANNEL, calculateStaticVolume());
  }
}

void AudioManager::handlePlayback() {}

void AudioManager::playMorseTone() {
  isPlayingMorse = true;
  isStaticPlaying = false;
  ledcAttachPin(Pins::SPEAKER, Audio::SPEAKER_CHANNEL);
  ledcWriteTone(Audio::SPEAKER_CHANNEL, MORSE_FREQUENCY);
  ledcWrite(Audio::SPEAKER_CHANNEL, currentVolume);
  g_events.push_back({true, millis()});
}

void AudioManager::stopMorseTone() {
  isPlayingMorse = false;
  ledcWrite(Audio::SPEAKER_CHANNEL, 0);
  ledcDetachPin(Pins::SPEAKER);
  g_events.push_back({false, millis()});
}

void AudioManager::updateStaticPattern() {
  ledcWriteTone(Audio::SPEAKER_CHANNEL, MORSE_FREQUENCY / 2);
  ledcWrite(Audio::SPEAKER_CHANNEL, calculateStaticVolume());
}

void AudioManager::playStaticNoise(int signalStrength) {
  isPlayingMorse = false;
  isStaticPlaying = true;
  currentSignalStrength = constrain(signalStrength, 0, 255);
  ledcAttachPin(Pins::SPEAKER, Audio::SPEAKER_CHANNEL);
  updateStaticPattern();
}

void AudioManager::stop() {
  isPlayingMorse = false;
  isStaticPlaying = false;
  ledcWrite(Audio::SPEAKER_CHANNEL, 0);
  ledcDetachPin(Pins::SPEAKER);
}
