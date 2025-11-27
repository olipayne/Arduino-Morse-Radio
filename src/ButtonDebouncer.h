#pragma once

#include <Arduino.h>

/**
 * Button Debouncer Class
 * Provides clean debouncing for digital button inputs with edge detection.
 * 
 * Features:
 * - Waits for stable state before accepting button press
 * - Detects press events (HIGH to LOW transition)
 * - Prevents retriggering while button is held
 * - Configurable debounce time
 */
class ButtonDebouncer {
 public:
  ButtonDebouncer(int pin, unsigned long debounceMs = 50, bool activeLow = true)
      : pin_(pin),
        debounceMs_(debounceMs),
        activeLow_(activeLow),
        lastReading_(HIGH),
        stableState_(HIGH),
        lastDebounceTime_(0),
        wasPressed_(false) {}

  void begin() {
    pinMode(pin_, INPUT_PULLUP);
    lastReading_ = digitalRead(pin_);
    stableState_ = lastReading_;
    lastDebounceTime_ = millis();
  }

  /**
   * Update the debouncer state. Call this regularly in your loop.
   */
  void update() {
    int currentReading = digitalRead(pin_);
    unsigned long currentTime = millis();

    // If the reading has changed, reset the debounce timer
    if (currentReading != lastReading_) {
      lastDebounceTime_ = currentTime;
    }

    // If the reading has been stable for longer than the debounce time,
    // accept it as the new stable state
    if ((currentTime - lastDebounceTime_) > debounceMs_) {
      // Detect state change
      if (currentReading != stableState_) {
        stableState_ = currentReading;
      }
    }

    lastReading_ = currentReading;
  }

  /**
   * Check if the button was just pressed (edge detection).
   * Returns true only once per press, on the falling edge (button down).
   * Must call update() before this in your loop.
   */
  bool wasPressed() {
    bool pressed = isPressed();
    
    if (pressed && !wasPressed_) {
      wasPressed_ = true;
      return true;
    }
    
    if (!pressed) {
      wasPressed_ = false;
    }
    
    return false;
  }

  /**
   * Check if the button is currently held down (level detection).
   */
  bool isPressed() const {
    return activeLow_ ? (stableState_ == LOW) : (stableState_ == HIGH);
  }

  /**
   * Check if the button was just released (edge detection).
   * Returns true only once per release, on the rising edge (button up).
   */
  bool wasReleased() {
    static bool wasHeld = false;
    bool held = isPressed();
    
    if (held) {
      wasHeld = true;
      return false;
    }
    
    if (wasHeld && !held) {
      wasHeld = false;
      return true;
    }
    
    return false;
  }

  /**
   * Get the raw debounced state (LOW or HIGH).
   */
  int getState() const { return stableState_; }

 private:
  const int pin_;
  const unsigned long debounceMs_;
  const bool activeLow_;
  
  int lastReading_;
  int stableState_;
  unsigned long lastDebounceTime_;
  bool wasPressed_;
};

