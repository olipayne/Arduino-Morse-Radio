#include "AudioManager.h"

void AudioManager::begin() {
  configurePWM();
  lastPulseTime = 0;

  // Initialize volume readings array
  for (int i = 0; i < VOLUME_SAMPLES; i++) {
    volumeReadings[i] = 0;
  }
  volumeIndex = 0;
  smoothedVolume = 0;
}

void AudioManager::configurePWM() {
  // Configure PWM with Morse frequency as base (higher than static noise)
  ledcSetup(Audio::SPEAKER_CHANNEL, MORSE_FREQUENCY, 8);
  ledcAttachPin(Pins::SPEAKER, Audio::SPEAKER_CHANNEL);
}

int AudioManager::smoothVolume(int newReading) {
  // Add new reading to array
  volumeReadings[volumeIndex] = newReading;
  volumeIndex = (volumeIndex + 1) % VOLUME_SAMPLES;

  // Calculate moving average
  long sum = 0;
  for (int i = 0; i < VOLUME_SAMPLES; i++) {
    sum += volumeReadings[i];
  }
  int average = sum / VOLUME_SAMPLES;

  // Only update if change is significant
  if (abs(average - smoothedVolume) > VOLUME_THRESHOLD) {
    smoothedVolume = average;
  }

  return smoothedVolume;
}

void AudioManager::setVolume(int adcValue) {
  // Apply smoothing to the ADC reading
  int smoothedADC = smoothVolume(adcValue);

  // Map smoothed value to volume range
  int newVolume = map(smoothedADC, 0, Radio::ADC_MAX, 0, 255);

  // Only update if volume has changed significantly
  if (abs(newVolume - currentVolume) > VOLUME_THRESHOLD) {
    currentVolume = newVolume;

    // If currently playing Morse tone or static, update volume
    if (isPlayingMorse || ledcRead(Audio::SPEAKER_CHANNEL) > 0) {
      ledcWrite(Audio::SPEAKER_CHANNEL, currentVolume);
    }
  }
}

void AudioManager::handlePlayback() {
  unsigned long currentTime = millis();
  if (currentTime - lastVolumeUpdate >= VOLUME_UPDATE_INTERVAL) {
    int volumeRead = analogRead(Pins::VOLUME_POT);
    setVolume(volumeRead);
    lastVolumeUpdate = currentTime;
  }
}

void AudioManager::playMorseTone() {
  isPlayingMorse = true;
  // Set exact 600Hz frequency for Morse code
  ledcWriteTone(Audio::SPEAKER_CHANNEL, MORSE_FREQUENCY);
  ledcWrite(Audio::SPEAKER_CHANNEL, currentVolume);
}

void AudioManager::stopMorseTone() {
  isPlayingMorse = false;
  ledcWrite(Audio::SPEAKER_CHANNEL, 0);
}

void AudioManager::playStaticNoise(int signalStrength) {
  isPlayingMorse = false;

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
}
