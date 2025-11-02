#include "AudioManager.h"
#include "PowerManager.h"

void AudioManager::begin() {
  configurePWM();
  lastPulseTime = 0;
  lastVolumeUpdate = 0;
  lastStaticPatternUpdate = 0;
  crackleEndTime = 0;
  currentVolume = 0;
  currentSignalStrength = 255;
  isPlayingMorse = false;
  isCrackling = false;
  staticBaseFrequency = MIN_STATIC_FREQ;
  isStaticPlaying = false;
}

void AudioManager::configurePWM() {
  // Configure PWM with Morse frequency as base (higher than static noise)
  ledcSetup(Audio::SPEAKER_CHANNEL, MORSE_FREQUENCY, 8);
  ledcAttachPin(Pins::SPEAKER, Audio::SPEAKER_CHANNEL);
}

void AudioManager::setVolume(int adcValue) {
  // Calculate the appropriate volume level based on the ADC reading
  int newVolume = calculateVolumeLevel(adcValue);

  // Handle volume changes
  if (newVolume != currentVolume) {
    // Store the new volume value
    currentVolume = newVolume;

    // If volume is zero, ensure audio is completely stopped
    if (currentVolume == 0) {
      // Completely silence the audio and detach the pin
      ledcWrite(Audio::SPEAKER_CHANNEL, 0);
      ledcDetachPin(Pins::SPEAKER);
    }
    // Only make sound if the volume is greater than zero
    else if (isPlayingMorse || isStaticPlaying) {
      // Reattach pin if needed
      ledcAttachPin(Pins::SPEAKER, Audio::SPEAKER_CHANNEL);

      // Update volume for morse tone
      if (isPlayingMorse) {
        ledcWrite(Audio::SPEAKER_CHANNEL, currentVolume);
      }
      // Update static noise with new volume - only update the volume, not the frequency
      else if (isStaticPlaying) {
        // Don't call updateStaticPattern() which would change frequency
        // Just update the volume directly
        int volumeToUse = calculateStaticVolume();
        ledcWrite(Audio::SPEAKER_CHANNEL, volumeToUse);
      }
    }
  }
}

/**
 * Calculates the appropriate volume level based on the ADC reading
 * @param adcValue Raw ADC value from the volume potentiometer
 * @return Calculated volume level (0-255)
 */
int AudioManager::calculateVolumeLevel(int adcValue) {
  // Handle zero volume case
  if (adcValue <= VOLUME_DEAD_ZONE) {
    return 0;  // Just return 0, the silencing will happen in setVolume
  }

  // Map ADC value to volume range, accounting for dead zone
  int newVolume = map(adcValue, VOLUME_DEAD_ZONE, Radio::ADC_MAX, 0, VOLUME_MAX);
  // Ensure volume stays in valid range
  return constrain(newVolume, 0, VOLUME_MAX);
}

// Helper method to calculate static volume without affecting frequency
int AudioManager::calculateStaticVolume() {
  // Invert signalStrength mapping - stronger signal = less static
  int staticIntensity = map(currentSignalStrength, 0, 255, 255, 0);

  // Volume scaling should only affect amplitude, not frequency
  int staticVolume = map(staticIntensity, 0, 255, 0, currentVolume);

  // Add some natural variation to the volume
  int volumeVariation = 0;
  if (!isCrackling) {
    volumeVariation = random(-5, 6);
  } else {
    // During crackle, apply the crackle volume variation
    int crackleIntensity = random(staticVolume / 2, int(staticVolume * 1.5));
    return constrain(crackleIntensity, 0, 255);
  }

  return constrain(staticVolume + volumeVariation, 0, 255);
}

void AudioManager::handlePlayback() {
  unsigned long currentTime = millis();

  // Update volume at regular intervals to avoid constant ADC reads
  // Cache the volume reading and only update if interval has passed
  if (currentTime - lastVolumeUpdate >= VOLUME_UPDATE_INTERVAL) {
    int volumeRead = PowerManager::getInstance().readADC(Pins::VOLUME_POT);
    // Only call setVolume if the reading actually changed (reduces unnecessary processing)
    static int lastVolumeRead = -1;
    if (abs(volumeRead - lastVolumeRead) > 10) {  // Only update if change is significant
      setVolume(volumeRead);
      lastVolumeRead = volumeRead;
    }
    lastVolumeUpdate = currentTime;
  }

  // Continuously update static noise if playing and volume is greater than zero
  // Skip updates if volume is zero to save power
  if (isStaticPlaying && !isPlayingMorse && currentVolume > 0) {
    updateStaticPattern();
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
  isStaticPlaying = true;
  currentSignalStrength = signalStrength;

  // Ensure PWM is attached
  ledcAttachPin(Pins::SPEAKER, Audio::SPEAKER_CHANNEL);

  // Update the static noise pattern
  updateStaticPattern();
}

void AudioManager::updateStaticPattern() {
  // Don't process anything if volume is zero
  if (currentVolume <= 0) {
    return;
  }

  unsigned long currentTime = millis();

  // Check if we need to update the static pattern
  // Only update at intervals to reduce CPU usage and power consumption
  if (currentTime - lastStaticPatternUpdate >= STATIC_PATTERN_CHANGE_INTERVAL) {
    // Calculate noise characteristics based on signal strength
    // Invert signalStrength mapping - stronger signal = less static
    int staticIntensity = map(currentSignalStrength, 0, 255, 255, 0);
    // Update base frequency periodically for a more natural sound
    // This should be independent of volume control

    // Calculate the midpoint of our static frequency range
    int midFrequency = (MIN_STATIC_FREQ + MAX_STATIC_FREQ) / 2;

    // If we've drifted too far from the center range, apply a correction
    // to pull back toward the center of our desired frequency range
    if (staticBaseFrequency > midFrequency + 50) {
      // If too high, bias toward decreasing
      staticBaseFrequency += random(-8, 4);
    } else if (staticBaseFrequency < midFrequency - 50) {
      // If too low, bias toward increasing
      staticBaseFrequency += random(-4, 8);
    } else {
      // Within reasonable range, use balanced random adjustment
      staticBaseFrequency += random(-5, 6);
    }

    // Hard boundaries to ensure we stay in range
    staticBaseFrequency = constrain(staticBaseFrequency, MIN_STATIC_FREQ, MAX_STATIC_FREQ);

    // Periodically reset to center range to prevent long-term drift
    // Reset roughly every 10 seconds (200 updates at 50ms intervals)
    if (random(200) == 0) {
      staticBaseFrequency = midFrequency + random(-25, 26);
    }

    lastStaticPatternUpdate = currentTime;

    // Determine if we should start a new crackle - based on signal strength only, not volume
    if (!isCrackling && random(0, 100) < map(staticIntensity, 0, 255, 0, MAX_CRACKLE_CHANCE)) {
      isCrackling = true;
      crackleEndTime = currentTime + random(CRACKLE_DURATION_MIN, CRACKLE_DURATION_MAX);
    }
  }

  int noiseFrequency;
  int volumeLevel;

  // Handle crackles that persist over multiple updates
  if (isCrackling) {
    if (currentTime < crackleEndTime) {
      // During crackle - use higher frequency and variable volume
      // Frequency range for crackles should be fixed, independent of volume
      noiseFrequency = random(MIN_STATIC_FREQ * 2, MAX_STATIC_FREQ * 3);
    } else {
      // Crackle has ended
      isCrackling = false;
      noiseFrequency = staticBaseFrequency;
    }
  } else {
    // Normal static noise with small random variations
    // Frequency variations should be independent of volume
    int randomOffset = random(-20, 21);
    noiseFrequency = staticBaseFrequency + randomOffset;
    noiseFrequency = constrain(noiseFrequency, MIN_STATIC_FREQ, MAX_STATIC_FREQ);
  }

  // Calculate volume separately from frequency
  volumeLevel = calculateStaticVolume();

  // Apply the frequency and volume
  ledcWriteTone(Audio::SPEAKER_CHANNEL, noiseFrequency);
  ledcWrite(Audio::SPEAKER_CHANNEL, volumeLevel);
}

void AudioManager::stop() {
  isPlayingMorse = false;
  isStaticPlaying = false;
  ledcWrite(Audio::SPEAKER_CHANNEL, 0);
  ledcDetachPin(Pins::SPEAKER);
}
