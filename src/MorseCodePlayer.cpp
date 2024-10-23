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
  this->message = message;
  messageIndex = 0;
  currentMorseCode = getMorseCode(message[messageIndex]);
  morseCodeIndex = 0;
  playing = true;
  timer = millis();
  toneOn = false;
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
        }
        else
        {
          // Finished
          playing = false;
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

String MorseCodePlayer::getMorseCode(char c)
{
  switch (toupper(c))
  {
  case 'A':
    return ".-";
  case 'B':
    return "-...";
  case 'C':
    return "-.-.";
  case 'D':
    return "-..";
  case 'E':
    return ".";
  case 'F':
    return "..-.";
  case 'G':
    return "--.";
  case 'H':
    return "....";
  case 'I':
    return "..";
  case 'J':
    return ".---";
  case 'K':
    return "-.-";
  case 'L':
    return ".-..";
  case 'M':
    return "--";
  case 'N':
    return "-.";
  case 'O':
    return "---";
  case 'P':
    return ".--.";
  case 'Q':
    return "--.-";
  case 'R':
    return ".-.";
  case 'S':
    return "...";
  case 'T':
    return "-";
  case 'U':
    return "..-";
  case 'V':
    return "...-";
  case 'W':
    return ".--";
  case 'X':
    return "-..-";
  case 'Y':
    return "-.--";
  case 'Z':
    return "--..";
  case '1':
    return ".----";
  case '2':
    return "..---";
  case '3':
    return "...--";
  case '4':
    return "....-";
  case '5':
    return ".....";
  case '6':
    return "-....";
  case '7':
    return "--...";
  case '8':
    return "---..";
  case '9':
    return "----.";
  case '0':
    return "-----";
  default:
    return ""; // Return empty string for unsupported characters
  }
}
