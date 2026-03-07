#include "SignalManager.h"

void SignalManager::begin() {
  // Initialize LOCK_LED as digital output
  pinMode(Pins::LOCK_LED, OUTPUT);
  digitalWrite(Pins::LOCK_LED, LOW);
  isLocked = false;
  currentSignalStrength = 0;

  // Initialize CARRIER_PWM using PWM
  ledcSetup(PWMChannels::CARRIER, LEDConfig::PWM_FREQUENCY, LEDConfig::PWM_RESOLUTION);
  ledcAttachPin(Pins::CARRIER_PWM, PWMChannels::CARRIER);
  ledcWrite(PWMChannels::CARRIER, 0);

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.println("SignalManager initialized");
#endif
}

void SignalManager::updateLockStatus(bool locked) {
  if (locked == isLocked) {
    return;
  }
  digitalWrite(Pins::LOCK_LED, locked ? HIGH : LOW);
  isLocked = locked;
}

void SignalManager::updateSignalStrength(int strength) {
  int pwmValue = constrain(strength, 0, 255);
  if (pwmValue == currentSignalStrength) {
    return;
  }
  ledcWrite(PWMChannels::CARRIER, pwmValue);
  currentSignalStrength = pwmValue;
}

void SignalManager::debugPrint(bool locked, const char* stationName, int signalStrength) {
#ifdef DEBUG_SERIAL_OUTPUT
  static unsigned long lastDebugPrint = 0;
  unsigned long currentTime = millis();

  // Only print debug info periodically to avoid flooding the serial output
  if (currentTime - lastDebugPrint >= 1000) {
    Serial.print("Signal: ");
    Serial.print(signalStrength);
    Serial.print(" | Locked: ");
    Serial.print(locked ? "YES" : "NO");

    if (stationName) {
      Serial.print(" | Station: ");
      Serial.print(stationName);
    } else {
      Serial.print(" | No station");
    }

    Serial.println();
    lastDebugPrint = currentTime;
  }
#endif
}
