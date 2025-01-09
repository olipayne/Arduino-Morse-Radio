#include "AudioManager.h"
#include "PowerManager.h"

void AudioManager::begin() {
  configurePWM();
  lastPulseTime = 0;
  lastVolumeUpdate = 0;
  currentVolume = 0;
  isPlayingMorse = false;
}

void AudioManager::configurePWM() {
  // Configure PWM with Morse frequency as base (higher than static noise)
  ledcSetup(Audio::SPEAKER_CHANNEL, MORSE_FREQUENCY, 8);
  ledcAttachPin(Pins::SPEAKER, Audio::SPEAKER_CHANNEL);
}

void AudioManager::setVolume(int adcValue) {
  // Handle zero volume case first
  if (adcValue <= VOLUME_DEAD_ZONE) {
    if (currentVolume > 0) {  // Only update if we need to turn off
      currentVolume = 0;
      ledcWrite(Audio::SPEAKER_CHANNEL, 0);
      ledcDetachPin(Pins::SPEAKER);
    }
    return;
  }

  // Map ADC value to volume range, accounting for dead zone
  int newVolume = map(adcValue, VOLUME_DEAD_ZONE, Radio::ADC_MAX, 0, VOLUME_MAX);
  newVolume = constrain(newVolume, 0, VOLUME_MAX);  // Ensure volume stays in valid range

  // Update volume if changed
  if (newVolume != currentVolume) {
    currentVolume = newVolume;
    
    // Ensure pin is attached and update volume if we're making sound
    if (currentVolume > 0 && (isPlayingMorse || ledcRead(Audio::SPEAKER_CHANNEL) > 0)) {
      ledcAttachPin(Pins::SPEAKER, Audio::SPEAKER_CHANNEL);
      ledcWrite(Audio::SPEAKER_CHANNEL, currentVolume);
    }
  }
}

void AudioManager::handlePlayback() {
  unsigned long currentTime = millis();
  if (currentTime - lastVolumeUpdate >= VOLUME_UPDATE_INTERVAL) {
    int volumeRead = PowerManager::getInstance().readADC(Pins::VOLUME_POT);
    setVolume(volumeRead);
    lastVolumeUpdate = currentTime;
  }
}

void AudioManager::playMorseTone() {
  isPlayingMorse = true;
  // Ensure PWM is attached
  ledcAttachPin(Pins::SPEAKER, Audio::SPEAKER_CHANNEL);
  // Set exact 600Hz frequency for Morse code
  ledcWriteTone(Audio::SPEAKER_CHANNEL, MORSE_FREQUENCY);
  ledcWrite(Audio::SPEAKER_CHANNEL, currentVolume);
}

void AudioManager::stopMorseTone() {
  isPlayingMorse = false;
  ledcWrite(Audio::SPEAKER_CHANNEL, 0);
  ledcDetachPin(Pins::SPEAKER);
}

void AudioManager::playStaticNoise(int signalStrength) {
  isPlayingMorse = false;

  // Ensure PWM is attached
  ledcAttachPin(Pins::SPEAKER, Audio::SPEAKER_CHANNEL);
  
  // Generate random noise within the frequency range
  int noiseFrequency = random(MIN_STATIC_FREQ, MAX_STATIC_FREQ + 1);

  // Invert signalStrength mapping
  int scaledVolume = map(signalStrength, 0, 255, currentVolume, 0);

  ledcWriteTone(Audio::SPEAKER_CHANNEL, noiseFrequency);
  ledcWrite(Audio::SPEAKER_CHANNEL, scaledVolume);
}

void AudioManager::stop() {
  isPlayingMorse = false;
  ledcWrite(Audio::SPEAKER_CHANNEL, 0);
  ledcDetachPin(Pins::SPEAKER);
}
