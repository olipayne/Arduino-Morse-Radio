#include "MorseCode.h"
#include "Config.h"
#include "AudioManager.h"

bool morsePlaying = false;
bool morseToneOn = false;
unsigned int dashDuration = 300;
unsigned int dotDuration = 100;
unsigned int partGap = 100;
unsigned int characterGap = 300;

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
    return "";
  }
}

void playMorseMessage(const String &message)
{
  Serial.print("Playing Morse Code Message: ");
  Serial.println(message);

  for (int i = 0; i < message.length(); i++)
  {
    String morseCode = getMorseCode(message[i]);

    for (int j = 0; j < morseCode.length(); j++)
    {
      bool isDash = (morseCode[j] == '-');
      int duration = isDash ? dashDuration : dotDuration;
      Serial.print(isDash ? "Dash " : "Dot ");
      Serial.print("on GPIO ");
      Serial.print(SPEAKER_PIN);
      Serial.print(" for ");
      Serial.print(duration);
      Serial.println(" ms");

      // Turn on the speaker (play tone)
      ledcWriteTone(SPEAKER_CHANNEL, 1000); // Set 1000 Hz tone frequency
      ledcWrite(SPEAKER_CHANNEL, 128);      // Set duty cycle for volume

      delay(duration);

      // Turn off the speaker between tones
      ledcWrite(SPEAKER_CHANNEL, 0);
      delay(partGap);
    }

    // Gap between characters
    delay(characterGap);
  }

  Serial.println("Morse Code Message Playback Complete");
}

void stopMorse()
{
  morsePlaying = false;
}

int calculateSignalStrength(int potValue, int targetValue)
{
  int difference = abs(potValue - targetValue);
  return difference <= LEEWAY ? map(difference, 0, LEEWAY, 255, 0) : 0;
}
