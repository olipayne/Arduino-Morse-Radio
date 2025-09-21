#include "MorseCode.h"

// Morse code patterns stored in PROGMEM to save RAM
static const char morse_A[] PROGMEM = ".-";
static const char morse_B[] PROGMEM = "-...";
static const char morse_C[] PROGMEM = "-.-.";
static const char morse_D[] PROGMEM = "-..";
static const char morse_E[] PROGMEM = ".";
static const char morse_F[] PROGMEM = "..-.";
static const char morse_G[] PROGMEM = "--.";
static const char morse_H[] PROGMEM = "....";
static const char morse_I[] PROGMEM = "..";
static const char morse_J[] PROGMEM = ".---";
static const char morse_K[] PROGMEM = "-.-";
static const char morse_L[] PROGMEM = ".-..";
static const char morse_M[] PROGMEM = "--";
static const char morse_N[] PROGMEM = "-.";
static const char morse_O[] PROGMEM = "---";
static const char morse_P[] PROGMEM = ".--.";
static const char morse_Q[] PROGMEM = "--.-";
static const char morse_R[] PROGMEM = ".-.";
static const char morse_S[] PROGMEM = "...";
static const char morse_T[] PROGMEM = "-";
static const char morse_U[] PROGMEM = "..-";
static const char morse_V[] PROGMEM = "...-";
static const char morse_W[] PROGMEM = ".--";
static const char morse_X[] PROGMEM = "-..-";
static const char morse_Y[] PROGMEM = "-.--";
static const char morse_Z[] PROGMEM = "--..";
static const char morse_0[] PROGMEM = "-----";
static const char morse_1[] PROGMEM = ".----";
static const char morse_2[] PROGMEM = "..---";
static const char morse_3[] PROGMEM = "...--";
static const char morse_4[] PROGMEM = "....-";
static const char morse_5[] PROGMEM = ".....";
static const char morse_6[] PROGMEM = "-....";
static const char morse_7[] PROGMEM = "--...";
static const char morse_8[] PROGMEM = "---..";
static const char morse_9[] PROGMEM = "----.";

const char* const MorseCode::MORSE_PATTERNS[] PROGMEM = {
    morse_A, morse_B, morse_C, morse_D, morse_E, morse_F, morse_G, morse_H, morse_I,
    morse_J, morse_K, morse_L, morse_M, morse_N, morse_O, morse_P, morse_Q, morse_R,
    morse_S, morse_T, morse_U, morse_V, morse_W, morse_X, morse_Y, morse_Z,
    morse_0, morse_1, morse_2, morse_3, morse_4, morse_5, morse_6, morse_7, morse_8, morse_9
};

// Add named constants for special characters
const char DOT = '.';
const char DASH = '-';
const char SPACE = ' ';

void MorseCode::begin() {
  // Initialize MORSE_LEDS as digital output
  pinMode(Pins::MORSE_LEDS, OUTPUT);
  updateMorseLEDs(false);
  stop();
}

void MorseCode::startMessage(const String& message) {
  startMessage(message.c_str());
}

void MorseCode::startMessage(const char* message) {
  stop();

  currentMessage = String(message);  // Still need String for compatibility, but minimize usage
  messageIndex = 0;
  symbolIndex = 0;
  inTuneInDelay = true;
  tuneInStartTime = millis();

  auto& config = ConfigManager::getInstance();
  config.setMorsePlaying(true);
  config.setMorseToneOn(false);
  updateMorseLEDs(false);

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.print(F("Starting Morse Message: "));
  Serial.println(message);
#endif
}

void MorseCode::update() {
  auto& config = ConfigManager::getInstance();
  auto& audio = AudioManager::getInstance();

  if (!config.isMorsePlaying()) return;

  unsigned long currentTime = millis();

  // Handle tune-in delay
  if (inTuneInDelay) {
    handleTuneInDelay(currentTime, config, audio);
    return;
  }

  const auto& timings = config.getCurrentMorseTimings();

  // If we need to start a new character
  if (symbolIndex == 0 || currentMorseChar.isEmpty()) {
    if (messageIndex >= currentMessage.length()) {
      stop();
      return;
    }

    // Get next character's morse code
    currentMorseChar = getSymbol(currentMessage[messageIndex]);
    if (currentMorseChar.isEmpty()) {
      messageIndex++;
      return;
    }
  }

  // Handle spaces between words
  if (currentMessage[messageIndex] == SPACE) {
    handleWordSpace(currentTime, timings, config, audio);
    return;
  }

  // Process morse symbols (dots and dashes)
  processSymbol(currentTime, timings, config, audio);
}

// Helper methods to break down the update function
void MorseCode::handleTuneInDelay(unsigned long currentTime, ConfigManager& config,
                                  AudioManager& audio) {
  config.setMorseToneOn(false);
  audio.stopMorseTone();
  updateMorseLEDs(false);

  if (currentTime - tuneInStartTime >= TUNE_IN_DELAY) {
    inTuneInDelay = false;
    lastStateChange = currentTime;
    // Start with first symbol ON
    if (messageIndex < currentMessage.length() && currentMessage[messageIndex] != SPACE) {
      currentMorseChar = getSymbol(currentMessage[messageIndex]);
      if (!currentMorseChar.isEmpty()) {
        config.setMorseToneOn(true);
        audio.playMorseTone();
        updateMorseLEDs(true);
      }
    }
  }
}

void MorseCode::handleWordSpace(unsigned long currentTime, const Audio::MorseTimings& timings,
                                ConfigManager& config, AudioManager& audio) {
  config.setMorseToneOn(false);
  audio.stopMorseTone();
  updateMorseLEDs(false);

  if (currentTime - lastStateChange >= timings.wordGap) {
    messageIndex++;
    symbolIndex = 0;
    lastStateChange = currentTime;
    // Start next character with symbol ON
    if (messageIndex < currentMessage.length()) {
      currentMorseChar = getSymbol(currentMessage[messageIndex]);
      if (!currentMorseChar.isEmpty()) {
        config.setMorseToneOn(true);
        audio.playMorseTone();
        updateMorseLEDs(true);
      }
    }
  }
}

void MorseCode::processSymbol(unsigned long currentTime, const Audio::MorseTimings& timings,
                              ConfigManager& config, AudioManager& audio) {
  // Get the current symbol (dot or dash)
  char currentSymbol = currentMorseChar[symbolIndex];
  bool isSymbolOn = config.isMorseToneOn();
  unsigned long symbolDuration =
      (currentSymbol == DASH) ? timings.dashDuration : timings.dotDuration;

  // Determine the appropriate gap duration
  unsigned long gapDuration;
  if (symbolIndex == currentMorseChar.length() - 1) {
    // Last symbol in character - use letter gap
    gapDuration = timings.letterGap;
  } else {
    // Between symbols - use symbol gap
    gapDuration = timings.symbolGap;
  }

  // Toggle the morse tone and LEDs based on timing
  if (isSymbolOn) {
    if (currentTime - lastStateChange >= symbolDuration) {
      config.setMorseToneOn(false);
      audio.stopMorseTone();
      updateMorseLEDs(false);
      lastStateChange = currentTime;
    }
  } else {
    if (currentTime - lastStateChange >= gapDuration) {
      symbolIndex++;
      if (symbolIndex >= currentMorseChar.length()) {
        // Move to next character
        messageIndex++;
        symbolIndex = 0;
        // Start next character with symbol ON if available
        if (messageIndex < currentMessage.length() && currentMessage[messageIndex] != SPACE) {
          currentMorseChar = getSymbol(currentMessage[messageIndex]);
          if (!currentMorseChar.isEmpty()) {
            config.setMorseToneOn(true);
            audio.playMorseTone();
            updateMorseLEDs(true);
          }
        }
      } else {
        // Start next symbol ON
        config.setMorseToneOn(true);
        audio.playMorseTone();
        updateMorseLEDs(true);
      }
      lastStateChange = currentTime;
    }
  }
}

void MorseCode::stop() {
  auto& config = ConfigManager::getInstance();
  auto& audio = AudioManager::getInstance();

  config.setMorsePlaying(false);
  config.setMorseToneOn(false);
  audio.stopMorseTone();
  updateMorseLEDs(false);

  messageIndex = 0;
  symbolIndex = 0;
  currentMorseChar.clear();
  inTuneInDelay = false;
}

void MorseCode::updateMorseLEDs(bool on) { digitalWrite(Pins::MORSE_LEDS, on ? HIGH : LOW); }

String MorseCode::getSymbol(char c) const {
  if (c == SPACE) return String(SPACE);

  c = toupper(c);
  if (c >= 'A' && c <= 'Z') {
    // Read pattern from PROGMEM
    const char* pattern = (const char*)pgm_read_ptr(&MORSE_PATTERNS[c - 'A']);
    return String(pattern);
  }
  if (c >= '0' && c <= '9') {
    // Read pattern from PROGMEM
    const char* pattern = (const char*)pgm_read_ptr(&MORSE_PATTERNS[26 + (c - '0')]);
    return String(pattern);
  }
  return "";  // Return empty string for unsupported characters
}
