// MorseCode.cpp
#include "MorseCode.h"

// Define the Morse code patterns lookup table
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
  stop(); // Ensure we start in a clean state
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

void MorseCode::playMessage(const String &message)
{
  auto &config = ConfigManager::getInstance();
  auto &audio = AudioManager::getInstance();

  if (message.isEmpty())
    return;

  config.setMorsePlaying(true);
  shouldStop = false;

  Serial.print("Playing Morse Code Message: ");
  Serial.println(message);

  for (size_t i = 0; i < message.length() && !shouldStop; i++)
  {
    String morseChar = getSymbol(message[i]);

    if (message[i] == SPACE)
    {
      wordGap();
      continue;
    }

    for (size_t j = 0; j < morseChar.length() && !shouldStop; j++)
    {
      playSymbol(morseChar[j]);
      if (j < morseChar.length() - 1)
      {
        symbolGap();
      }
    }

    if (i < message.length() - 1 && message[i + 1] != SPACE)
    {
      letterGap();
    }
  }

  config.setMorsePlaying(false);
  config.setMorseToneOn(false);
  audio.stop();
}

void MorseCode::playSymbol(char symbol)
{
  auto &config = ConfigManager::getInstance();
  auto &audio = AudioManager::getInstance();

  config.setMorseToneOn(true);
  audio.playMorseTone();

  if (symbol == DOT)
  {
    dotDelay();
  }
  else if (symbol == DASH)
  {
    dashDelay();
  }

  config.setMorseToneOn(false);
  audio.stopMorseTone();
}

void MorseCode::stop()
{
  shouldStop = true;
  auto &config = ConfigManager::getInstance();
  auto &audio = AudioManager::getInstance();

  config.setMorsePlaying(false);
  config.setMorseToneOn(false);
  audio.stop();
}

void MorseCode::dotDelay()
{
  const auto &timings = ConfigManager::getInstance().getCurrentMorseTimings();
  delay(timings.dotDuration);
}

void MorseCode::dashDelay()
{
  const auto &timings = ConfigManager::getInstance().getCurrentMorseTimings();
  delay(timings.dashDuration);
}

void MorseCode::symbolGap()
{
  const auto &timings = ConfigManager::getInstance().getCurrentMorseTimings();
  delay(timings.symbolGap);
}

void MorseCode::letterGap()
{
  const auto &timings = ConfigManager::getInstance().getCurrentMorseTimings();
  delay(timings.letterGap);
}

void MorseCode::wordGap()
{
  const auto &timings = ConfigManager::getInstance().getCurrentMorseTimings();
  delay(timings.wordGap);
}

void MorseCode::printMessage(const String &message)
{
  Serial.print("Message '");
  Serial.print(message);
  Serial.print("' in Morse code: ");

  for (size_t i = 0; i < message.length(); i++)
  {
    String morseChar = getSymbol(message[i]);
    Serial.print(morseChar);
    if (i < message.length() - 1)
    {
      Serial.print(" ");
    }
  }
  Serial.println();
}