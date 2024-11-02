// MorseCodePlayer.cpp
#include "MorseCodePlayer.h"

// Constructor
MorseCodePlayer::MorseCodePlayer()
{
  setSpeed(100); // Default dot duration
  playing = false;
  frequency = 800; // Default Morse code frequency
  volume = 64;     // Default volume
}

void MorseCodePlayer::setMessage(const String &message)
{
  if (message.length() == 0)
  {
    // If the message is empty, stop playing
    stop();
    return;
  }

  this->message = message;
  messageIndex = 0;
  currentMorseCode = getMorseCode(message[messageIndex]);
  morseCodeIndex = 0;
  playing = true;
  timer = millis();
  toneOn = false;

  // Serial logging for starting a new message
  Serial.print("Starting Morse code message: ");
  Serial.println(message);

  // Serial logging for the first character
  Serial.print("Starting character: ");
  Serial.print(message[messageIndex]);
  Serial.print(" Morse Code: ");
  Serial.println(currentMorseCode);
}

void MorseCodePlayer::setSpeed(unsigned int dotDuration)
{
  this->dotDuration = dotDuration;
  dashDuration = dotDuration * 3;
  partGap = dotDuration;
  characterGap = dotDuration * 3;
}

void MorseCodePlayer::update()
{
  if (!playing)
    return;

  unsigned long currentTime = millis();

  if (toneOn)
  {
    // Turn off tone after duration
    unsigned int duration = (currentMorseCode[morseCodeIndex] == '.') ? dotDuration : dashDuration;
    if (currentTime - timer >= duration)
    {
      // Turn off tone
      toneOn = false;
      morseCodeIndex++;
      timer = currentTime;
    }
  }
  else
  {
    // Turn on tone if there are more symbols
    if (morseCodeIndex < currentMorseCode.length())
    {
      // Wait for part gap
      if (currentTime - timer >= partGap)
      {
        // Turn on tone
        toneOn = true;
        timer = currentTime;
      }
    }
    else
    {
      // Move to next character
      if (currentTime - timer >= characterGap)
      {
        messageIndex++;
        if (messageIndex < message.length())
        {
          currentMorseCode = getMorseCode(message[messageIndex]);
          morseCodeIndex = 0;

          // Serial logging for starting a new character
          Serial.print("Starting character: ");
          Serial.print(message[messageIndex]);
          Serial.print(" Morse Code: ");
          Serial.println(currentMorseCode);
        }
        else
        {
          // Finished
          playing = false;

          // Serial logging for message completion
          Serial.println("Morse code message playback finished.");
        }
        timer = currentTime;
      }
    }
  }
}

bool MorseCodePlayer::isPlaying() const
{
  return playing;
}

bool MorseCodePlayer::isToneOn() const
{
  return toneOn;
}

unsigned int MorseCodePlayer::getFrequency() const
{
  return frequency;
}

void MorseCodePlayer::setFrequency(unsigned int frequency)
{
  this->frequency = frequency;
}

void MorseCodePlayer::setVolume(unsigned int volume)
{
  this->volume = volume;
}

unsigned int MorseCodePlayer::getVolume() const
{
  return volume;
}

void MorseCodePlayer::stop()
{
  playing = false;
  toneOn = false;
}

const struct
{
  char character;
  const char *code;
} MORSE_CODE_MAP[] PROGMEM = {
    {'A', ".-"}, {'B', "-..."}, {'C', "-.-."}, {'D', "-.."}, {'E', "."}, {'F', "..-."}, {'G', "--."}, {'H', "...."}, {'I', ".."}, {'J', ".---"}, {'K', "-.-"}, {'L', ".-.."}, {'M', "--"}, {'N', "-."}, {'O', "---"}, {'P', ".--."}, {'Q', "--.-"}, {'R', ".-."}, {'S', "..."}, {'T', "-"}, {'U', "..-"}, {'V', "...-"}, {'W', ".--"}, {'X', "-..-"}, {'Y', "-.--"}, {'Z', "--.."}, {'1', ".----"}, {'2', "..---"}, {'3', "...--"}, {'4', "....-"}, {'5', "....."}, {'6', "-...."}, {'7', "--..."}, {'8', "---.."}, {'9', "----."}, {'0', "-----"}};

String MorseCodePlayer::getMorseCode(char c)
{
  c = toupper(c);
  for (const auto &entry : MORSE_CODE_MAP)
  {
    if (entry.character == c)
    {
      return String(entry.code);
    }
  }
  return "";
}