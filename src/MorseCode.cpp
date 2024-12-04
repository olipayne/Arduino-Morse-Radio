#include "MorseCode.h"

const char *const MorseCode::MORSE_PATTERNS[] = {
    ".-",    // A
    "-...",  // B
    "-.-.",  // C
    "-..",   // D
    ".",     // E
    "..-.",  // F
    "--.",   // G
    "....",  // H
    "..",    // I
    ".---",  // J
    "-.-",   // K
    ".-..",  // L
    "--",    // M
    "-.",    // N
    "---",   // O
    ".--.",  // P
    "--.-",  // Q
    ".-.",   // R
    "...",   // S
    "-",     // T
    "..-",   // U
    "...-",  // V
    ".--",   // W
    "-..-",  // X
    "-.--",  // Y
    "--..",  // Z
    "-----", // 0
    ".----", // 1
    "..---", // 2
    "...--", // 3
    "....-", // 4
    ".....", // 5
    "-....", // 6
    "--...", // 7
    "---..", // 8
    "----."  // 9
};

void MorseCode::begin()
{
  // Initialize MORSE_LEDS as digital output
  pinMode(Pins::MORSE_LEDS, OUTPUT);
  updateMorseLEDs(false);
  stop();
}

void MorseCode::startMessage(const String &message)
{
  stop();

  currentMessage = message;
  messageIndex = 0;
  symbolIndex = 0;
  inTuneInDelay = true;
  tuneInStartTime = millis();

  auto &config = ConfigManager::getInstance();
  config.setMorsePlaying(true);
  config.setMorseToneOn(false);
  updateMorseLEDs(false);

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.print("Starting Morse Message: ");
  Serial.println(message);
#endif
}

void MorseCode::update()
{
  auto &config = ConfigManager::getInstance();
  auto &audio = AudioManager::getInstance();

  if (!config.isMorsePlaying())
    return;

  unsigned long currentTime = millis();

  // Handle tune-in delay
  if (inTuneInDelay)
  {
    config.setMorseToneOn(false);
    audio.stopMorseTone();
    updateMorseLEDs(false);

    if (currentTime - tuneInStartTime >= TUNE_IN_DELAY)
    {
      inTuneInDelay = false;
      lastStateChange = currentTime;
      // Start with first symbol ON
      if (messageIndex < currentMessage.length() && currentMessage[messageIndex] != ' ')
      {
        currentMorseChar = getSymbol(currentMessage[messageIndex]);
        if (!currentMorseChar.isEmpty())
        {
          config.setMorseToneOn(true);
          audio.playMorseTone();
          updateMorseLEDs(true);
        }
      }
    }
    return;
  }

  const auto &timings = config.getCurrentMorseTimings();

  // If we need to start a new character
  if (symbolIndex == 0 || currentMorseChar.isEmpty())
  {
    if (messageIndex >= currentMessage.length())
    {
      stop();
      return;
    }

    // Get next character's morse code
    currentMorseChar = getSymbol(currentMessage[messageIndex]);
    if (currentMorseChar.isEmpty())
    {
      messageIndex++;
      return;
    }
  }

  // Handle spaces between words
  if (currentMessage[messageIndex] == ' ')
  {
    config.setMorseToneOn(false);
    audio.stopMorseTone();
    updateMorseLEDs(false);

    if (currentTime - lastStateChange >= timings.wordGap)
    {
      messageIndex++;
      symbolIndex = 0;
      lastStateChange = currentTime;
      // Start next character with symbol ON
      if (messageIndex < currentMessage.length())
      {
        currentMorseChar = getSymbol(currentMessage[messageIndex]);
        if (!currentMorseChar.isEmpty())
        {
          config.setMorseToneOn(true);
          audio.playMorseTone();
          updateMorseLEDs(true);
        }
      }
    }
    return;
  }

  // Get the current symbol (dot or dash)
  char currentSymbol = currentMorseChar[symbolIndex];
  bool isSymbolOn = config.isMorseToneOn();
  unsigned long symbolDuration = (currentSymbol == '-') ? timings.dashDuration : timings.dotDuration;

  // Determine the appropriate gap duration
  unsigned long gapDuration;
  if (symbolIndex == currentMorseChar.length() - 1)
  {
    // Last symbol in character - use letter gap
    gapDuration = timings.letterGap;
  }
  else
  {
    // Between symbols - use symbol gap
    gapDuration = timings.symbolGap;
  }

  // Toggle the morse tone and LEDs based on timing
  if (isSymbolOn)
  {
    if (currentTime - lastStateChange >= symbolDuration)
    {
      config.setMorseToneOn(false);
      audio.stopMorseTone();
      updateMorseLEDs(false);
      lastStateChange = currentTime;
    }
  }
  else
  {
    if (currentTime - lastStateChange >= gapDuration)
    {
      symbolIndex++;
      if (symbolIndex >= currentMorseChar.length())
      {
        // Move to next character
        messageIndex++;
        symbolIndex = 0;
        // Start next character with symbol ON if available
        if (messageIndex < currentMessage.length() && currentMessage[messageIndex] != ' ')
        {
          currentMorseChar = getSymbol(currentMessage[messageIndex]);
          if (!currentMorseChar.isEmpty())
          {
            config.setMorseToneOn(true);
            audio.playMorseTone();
            updateMorseLEDs(true);
          }
        }
      }
      else
      {
        // Start next symbol ON
        config.setMorseToneOn(true);
        audio.playMorseTone();
        updateMorseLEDs(true);
      }
      lastStateChange = currentTime;
    }
  }
}

void MorseCode::stop()
{
  auto &config = ConfigManager::getInstance();
  auto &audio = AudioManager::getInstance();

  config.setMorsePlaying(false);
  config.setMorseToneOn(false);
  audio.stopMorseTone();
  updateMorseLEDs(false);

  messageIndex = 0;
  symbolIndex = 0;
  currentMorseChar.clear();
  inTuneInDelay = false;
}

void MorseCode::updateMorseLEDs(bool on)
{
  digitalWrite(Pins::MORSE_LEDS, on ? HIGH : LOW);
}

String MorseCode::getSymbol(char c) const
{
  if (c == ' ')
    return " ";

  c = toupper(c);
  if (c >= 'A' && c <= 'Z')
  {
    return MORSE_PATTERNS[c - 'A'];
  }
  if (c >= '0' && c <= '9')
  {
    return MORSE_PATTERNS[26 + (c - '0')];
  }
  return ""; // Return empty string for unsupported characters
}
