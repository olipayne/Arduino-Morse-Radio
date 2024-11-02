#include "MorseCode.h"
#include "Config.h"

namespace MorseCode
{
  using namespace Config;

  // Morse code dictionary
  String getMorseCode(char c)
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

  // Function to play static noise
  void playStaticNoise(int overallSignalStrength)
  {
    int noiseFrequency = random(100, 300); // Random frequency between 100Hz and 300Hz
    int noiseVolume = speakerDutyCycle;    // Use the same volume as the buzzer

    // Play static noise
    ledcWriteTone(SPEAKER_CHANNEL, noiseFrequency);
    ledcWrite(SPEAKER_CHANNEL, noiseVolume);
  }

  // Function to calculate signal strength based on proximity to station frequency
  int calculateSignalStrength(int potValue, int targetValue)
  {
    int difference = abs(potValue - targetValue);
    if (difference <= LEEWAY)
    {
      return map(difference, 0, LEEWAY, 255, 0); // Stronger signal as difference approaches 0
    }
    else
    {
      return 0; // No signal outside of leeway
    }
  }
} // namespace MorseCode
