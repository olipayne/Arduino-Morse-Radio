#include "MorseCode.h"
#include <Preferences.h>
#include "Config.h"

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

// Function to set Morse code speed
void setMorseSpeed(MorseSpeed speed)
{
  morseSpeed = speed;

  int multiplier = 3; // Multiplier for dash and character gap

  switch (morseSpeed)
  {
  case LOW_SPEED:
    dotDuration = 500;
    break;
  case MEDIUM_SPEED:
    dotDuration = 300;
    break;
  case HIGH_SPEED:
    dotDuration = 100;
    break;
  }

  dashDuration = dotDuration * multiplier;
  partGap = dotDuration;
  characterGap = dotDuration * multiplier;
}

// Function to load configurations from non-volatile storage
void loadConfigurations()
{
  preferences.begin("config", false);

  londonMessage = preferences.getString("londonMsg", "L");
  hilversumMessage = preferences.getString("hilversumMsg", "H");
  barcelonaMessage = preferences.getString("barcelonaMsg", "B");
  speakerDutyCycle = preferences.getUInt("volume", 64);
  morseFrequency = preferences.getUInt("frequency", 800);
  morseSpeed = static_cast<MorseSpeed>(preferences.getUInt("morseSpeed", MEDIUM_SPEED));

  preferences.end();

  // Update Morse code timing
  setMorseSpeed(morseSpeed);
}

// Function to save configurations to non-volatile storage
void saveConfigurations()
{
  preferences.begin("config", false);

  preferences.putString("londonMsg", londonMessage);
  preferences.putString("hilversumMsg", hilversumMessage);
  preferences.putString("barcelonaMsg", barcelonaMessage);
  preferences.putUInt("volume", speakerDutyCycle);
  preferences.putUInt("frequency", morseFrequency);
  preferences.putUInt("morseSpeed", morseSpeed);

  preferences.end();
}

// Function to play static noise
void playStaticNoise(int overallSignalStrength)
{
  // Adjust static noise volume based on overall signal strength
  int maxNoiseVolume = map(overallSignalStrength, 0, 255, speakerDutyCycle / 2, 10);
  int noiseFrequency = random(100, 300); // Random frequency between 100Hz and 2000Hz
  int noiseVolume = random(10, maxNoiseVolume);

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
