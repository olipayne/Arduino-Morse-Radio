#pragma once

class PotentiometerReader {
 public:
  static constexpr int WINDOW_SIZE = 5;  // Size of moving average window

  PotentiometerReader(int pin, int hysteresis = 10)
      : pin_(pin), hysteresis_(hysteresis), lastStableValue_(0), windowIndex_(0) {
    for (int i = 0; i < WINDOW_SIZE; i++) {
      window_[i] = 0;
    }
  }

  void begin() {
    // Initialize all window values with first reading
    int initial = readRaw();
    for (int i = 0; i < WINDOW_SIZE; i++) {
      window_[i] = initial;
    }
    lastStableValue_ = initial;
  }

  // Get filtered reading with hysteresis and moving average
  int read() {
    // Add new reading to window
    window_[windowIndex_] = readRaw();
    windowIndex_ = (windowIndex_ + 1) % WINDOW_SIZE;

    // Calculate moving average
    long sum = 0;
    for (int i = 0; i < WINDOW_SIZE; i++) {
      sum += window_[i];
    }
    int currentValue = sum / WINDOW_SIZE;

    // Apply hysteresis with adaptive threshold
    int difference = currentValue - lastStableValue_;
    int adaptiveThreshold = hysteresis_;

    // Increase threshold for larger changes to prevent oscillation
    if (abs(difference) > hysteresis_ * 2) {
      adaptiveThreshold = hysteresis_ * 1.5;
    }

    // Only update if change exceeds threshold
    if (abs(difference) > adaptiveThreshold) {
      // Gradual change for smoother transitions
      lastStableValue_ +=
          (difference > 0) ? min(difference, hysteresis_ * 2) : max(difference, -hysteresis_ * 2);
    }

    return lastStableValue_;
  }

  // Get raw reading without filtering
  int readRaw() { return analogRead(pin_); }

 private:
  const int pin_;
  const int hysteresis_;
  int lastStableValue_;
  int window_[WINDOW_SIZE];
  int windowIndex_;
};
