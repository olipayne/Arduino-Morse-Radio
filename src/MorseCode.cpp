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

    if (currentTime - tuneInStartTime >= TUNE_IN_DELAY)
    {
      inTuneInDelay = false;
      lastStateChange = currentTime;
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

    if (currentTime - lastStateChange >= timings.wordGap)
    {
      messageIndex++;
      symbolIndex = 0;
      lastStateChange = currentTime;
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

  // Toggle the morse tone on/off based on timing
  if (isSymbolOn)
  {
    if (currentTime - lastStateChange >= symbolDuration)
    {
      config.setMorseToneOn(false);
      audio.stopMorseTone();
      lastStateChange = currentTime;
    }
  }
  else
  {
    if (currentTime - lastStateChange >= gapDuration)
    {
      if (symbolIndex >= currentMorseChar.length())
      {
        // Move to next character
        messageIndex++;
        symbolIndex = 0;
      }
      else
      {
        // Start next symbol
        config.setMorseToneOn(true);
        audio.playMorseTone();
        symbolIndex++;
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

  messageIndex = 0;
  symbolIndex = 0;
  currentMorseChar.clear();
  inTuneInDelay = false;
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
