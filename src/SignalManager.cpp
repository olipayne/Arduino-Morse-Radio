#include "SignalManager.h"

void SignalManager::begin() {
  // Initialize LOCK_LED as digital output
  pinMode(Pins::LOCK_LED, OUTPUT);
  digitalWrite(Pins::LOCK_LED, LOW);
  isStationLocked = false;
  previousLockState = false;
  lastLockPrintTime = 0;

  // Initialize CARRIER_PWM using PWM
  ledcSetup(PWMChannels::CARRIER, LEDConfig::PWM_FREQUENCY, LEDConfig::PWM_RESOLUTION);
  ledcAttachPin(Pins::CARRIER_PWM, PWMChannels::CARRIER);
  ledcWrite(PWMChannels::CARRIER, 0);

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.println("SignalManager initialized");
#endif
}

void SignalManager::updateLockStatus(bool locked) {
  if (locked != isStationLocked) {
    isStationLocked = locked;
    digitalWrite(Pins::LOCK_LED, locked ? HIGH : LOW);
  }
}

void SignalManager::updateSignalStrength(int strength) {
  ledcWrite(PWMChannels::CARRIER, strength);
}

#ifdef DEBUG_SERIAL_OUTPUT
void SignalManager::debugPrint(bool isLocked, const char* stationName, int signalStrength) {
  unsigned long currentTime = millis();
  bool lockStateChanged = (isLocked != previousLockState);
  bool timeToUpdate = (currentTime - lastLockPrintTime >= 5000);

  if (isLocked && (lockStateChanged || timeToUpdate)) {
    Serial.printf("Station locked: %s, Signal strength: %d\n",
                  stationName ? stationName : "Unknown", signalStrength);
    lastLockPrintTime = currentTime;
  } else if (!isLocked && lockStateChanged) {
    Serial.println("Station unlocked");
    lastLockPrintTime = currentTime;
  }

  previousLockState = isLocked;
}
#endif
