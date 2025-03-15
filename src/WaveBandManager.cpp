#include "WaveBandManager.h"

// Define the LED mapping
const WaveBandManager::BandLED WaveBandManager::BAND_LEDS[] = {
    {WaveBand::LONG_WAVE, Pins::LW_LED},
    {WaveBand::MEDIUM_WAVE, Pins::MW_LED},
    {WaveBand::SHORT_WAVE, Pins::SW_LED}};

void WaveBandManager::begin() {
  initializePins();
  update();  // Initial update
}

void WaveBandManager::initializePins() {
  // Initialize switch pins with pullups
  pinMode(Pins::LW_BAND_SWITCH, INPUT_PULLUP);
  pinMode(Pins::MW_BAND_SWITCH, INPUT_PULLUP);

  // Initialize LED pins as digital outputs
  for (size_t i = 0; i < NUM_BANDS; i++) {
    pinMode(BAND_LEDS[i].pin, OUTPUT);
    digitalWrite(BAND_LEDS[i].pin, LOW);
  }
}

void WaveBandManager::update() {
  auto& config = ConfigManager::getInstance();
  WaveBand newBand;

  // Read switch states
  if (digitalRead(Pins::LW_BAND_SWITCH) == LOW) {
    // LONG_WAVE switch works as expected
    newBand = WaveBand::LONG_WAVE;
  } else if (digitalRead(Pins::MW_BAND_SWITCH) == LOW) {
    // Swapped: MW switch now selects SHORT_WAVE
    // This is because the hardware was changed from rotary to toggle switches
    newBand = WaveBand::SHORT_WAVE;
  } else {
    // Swapped: Default (no switch pressed) is now MEDIUM_WAVE
    // This is because the hardware was changed from rotary to toggle switches
    newBand = WaveBand::MEDIUM_WAVE;
  }

  // Only update if band has changed
  if (newBand != config.getWaveBand()) {
    config.setWaveBand(newBand);
    updateLEDs();

#ifdef DEBUG_SERIAL_OUTPUT
    Serial.print("Wave band changed to: ");
    Serial.println(toString(newBand));
#endif
  }
}

WaveBand WaveBandManager::getCurrentBand() const {
  return ConfigManager::getInstance().getWaveBand();
}

void WaveBandManager::updateLEDs() {
  turnOffAllBandLEDs();
  updateBandLED(getCurrentBand());
}

void WaveBandManager::turnOffAllBandLEDs() {
  for (size_t i = 0; i < NUM_BANDS; i++) {
    digitalWrite(BAND_LEDS[i].pin, LOW);
  }
}

void WaveBandManager::updateBandLED(WaveBand band) {
  for (size_t i = 0; i < NUM_BANDS; i++) {
    if (BAND_LEDS[i].band == band) {
      digitalWrite(BAND_LEDS[i].pin, HIGH);
      break;
    }
  }
}

void WaveBandManager::setLEDBrightness(uint8_t brightness) {
  // No brightness control in digital mode
  // Just update LEDs to maintain current state
  updateLEDs();
}
