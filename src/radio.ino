#include <Arduino.h>
#include <Preferences.h>
#include "Config.h"
#include "Stations.h"
#include "MorseCode.h"
#include "WiFiManager.h"

// Hardware pin definitions
#define POTENTIOMETER_PIN A0     // Potentiometer pin (ADC input)
#define LOCK_LED_PIN LED_BUILTIN // Lock LED pin
#define SPEAKER_PIN D7           // Speaker pin for Morse code output
#define BLUE_LED_PIN LED_BLUE    // Blue LED pin for Wi-Fi status
#define WIFI_BUTTON_PIN D2       // Wi-Fi toggle button pin

// PWM configurations
const int PWM_FREQUENCY = 5000; // PWM frequency in Hz
const int PWM_RESOLUTION = 8;   // PWM resolution (8-bit)
const int LOCK_LED_CHANNEL = 0; // PWM channel for lock LED
const int SPEAKER_CHANNEL = 1;  // PWM channel for speaker

// Leeway for station locking
const int LEEWAY = 100; // Range around each station frequency

// Variables for speaker settings
unsigned int speakerDutyCycle = 64; // Speaker volume (duty cycle)
unsigned int morseFrequency = 500;  // Speaker frequency (Hz)

// Morse code playback variables
String currentMorseMessage = "";
int currentMorseIndex = 0;
String currentMorseCode = "";
int currentMorseCodeIndex = 0;
unsigned long morseTimer = 0;
bool morsePlaying = false;
bool morseToneOn = false;

// Morse code speed settings
MorseSpeed morseSpeed = MEDIUM_SPEED; // Default Morse code speed

// Timing variables (milliseconds)
unsigned int dotDuration = 100;
unsigned int dashDuration = 300;
unsigned int partGap = 100;
unsigned int characterGap = 500;

// Configuration variables
String londonMessage = "L";
String hilversumMessage = "H";
String barcelonaMessage = "B";

// Preferences for storing configurations
Preferences preferences;

// Wi-Fi control variables
bool wifiEnabled = false;          // Wi-Fi is off by default
unsigned long lastButtonPress = 0; // For debouncing the Wi-Fi button

// Externally declared hardware pins
const int blueLEDPin = BLUE_LED_PIN;

void setup()
{
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize hardware pins
  pinMode(POTENTIOMETER_PIN, INPUT);
  pinMode(LOCK_LED_PIN, OUTPUT);
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);
  pinMode(WIFI_BUTTON_PIN, INPUT_PULLUP); // Enable internal pull-up resistor

  // Turn off the blue LED initially (Wi-Fi is off)
  digitalWrite(blueLEDPin, HIGH); // Active-low configuration

  // Configure PWM channels
  ledcSetup(LOCK_LED_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(LOCK_LED_PIN, LOCK_LED_CHANNEL);

  ledcSetup(SPEAKER_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(SPEAKER_PIN, SPEAKER_CHANNEL);

  // Initially set the speaker to silent
  ledcWrite(SPEAKER_CHANNEL, 0);

  // Load saved configurations
  loadConfigurations();

  // Set initial Morse code speed
  setMorseSpeed(morseSpeed);

  // Initialize Wi-Fi manager
  initWiFiManager();
}

void loop()
{
  // Read potentiometer value (0-4095 for ESP32 ADC)
  int potValue = analogRead(POTENTIOMETER_PIN);

  unsigned long currentTime = millis();
  bool stationLocked = false;
  int lockBrightness = 0;
  Station *lockedStation = nullptr;
  int signalStrength = 0;

  // Determine the closest station
  for (int i = 0; i < numStations; i++)
  {
    int strength = calculateSignalStrength(potValue, stations[i].frequency);

    if (strength > 0)
    {
      // Signal detected for this station
      if (strength > signalStrength)
      {
        signalStrength = strength;
        lockBrightness = strength;
        stationLocked = true;
        lockedStation = &stations[i];
      }
    }
  }

  // Set the lock LED brightness
  ledcWrite(LOCK_LED_CHANNEL, lockBrightness);

  if (stationLocked)
  {
    // Adjust speaker volume based on signal strength
    unsigned int adjustedVolume = map(signalStrength, 0, 255, 0, speakerDutyCycle);

    if (!morsePlaying)
    {
      // Set the Morse message based on the locked station
      if (strcmp(lockedStation->name, "London") == 0)
      {
        currentMorseMessage = londonMessage;
      }
      else if (strcmp(lockedStation->name, "Hilversum") == 0)
      {
        currentMorseMessage = hilversumMessage;
      }
      else if (strcmp(lockedStation->name, "Barcelona") == 0)
      {
        currentMorseMessage = barcelonaMessage;
      }
      else
      {
        currentMorseMessage = "S"; // Default message
      }

      // Initialize Morse code playback variables
      currentMorseIndex = 0;
      currentMorseCodeIndex = 0;
      currentMorseCode = getMorseCode(currentMorseMessage[currentMorseIndex]);
      morsePlaying = true;
      morseTimer = currentTime;
      morseToneOn = false;
    }

    // Process Morse code playback
    if (morsePlaying && currentTime >= morseTimer)
    {
      if (morseToneOn)
      {
        // Turn off the tone
        ledcWrite(SPEAKER_CHANNEL, 0);
        morseToneOn = false;

        // Determine the appropriate gap
        if (currentMorseCodeIndex < currentMorseCode.length())
        {
          // Gap between parts of the same character
          morseTimer = currentTime + partGap;
        }
        else
        {
          // Gap between characters
          morseTimer = currentTime + characterGap - partGap;
        }
      }
      else
      {
        if (currentMorseCodeIndex < currentMorseCode.length())
        {
          // Play the next dot or dash
          char symbol = currentMorseCode[currentMorseCodeIndex];
          if (symbol == '.' || symbol == '-')
          {
            // Play dot or dash
            ledcWriteTone(SPEAKER_CHANNEL, morseFrequency);
            // Adjust volume based on signal strength
            ledcWrite(SPEAKER_CHANNEL, adjustedVolume);

            morseToneOn = true;
            if (symbol == '.')
            {
              morseTimer = currentTime + dotDuration;
            }
            else
            {
              morseTimer = currentTime + dashDuration;
            }
          }
          currentMorseCodeIndex++;
        }
        else
        {
          // Move to the next character
          currentMorseIndex++;
          currentMorseCodeIndex = 0;

          if (currentMorseIndex < currentMorseMessage.length())
          {
            currentMorseCode = getMorseCode(currentMorseMessage[currentMorseIndex]);
            // No need to set morseTimer here
          }
          else
          {
            // Finished the message
            morsePlaying = false;
            ledcWrite(SPEAKER_CHANNEL, 0);
          }
        }
      }
    }
  }
  else
  {
    // No station locked; play static noise
    morsePlaying = false;

    // Calculate overall signal strength (proximity to any station)
    int overallSignalStrength = 0;
    for (int i = 0; i < numStations; i++)
    {
      int strength = calculateSignalStrength(potValue, stations[i].frequency);
      if (strength > overallSignalStrength)
      {
        overallSignalStrength = strength;
      }
    }

    playStaticNoise(overallSignalStrength);
  }

  // Handle Wi-Fi button press (debounced)
  if (digitalRead(WIFI_BUTTON_PIN) == LOW)
  {
    if (currentTime - lastButtonPress > 500)
    { // 500 ms debounce
      toggleWiFi();
      lastButtonPress = currentTime;
    }
  }

  // Handle Wi-Fi tasks if enabled
  if (wifiEnabled)
  {
    handleWiFi();
  }

  // Small delay for stability
  delay(10);
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

// Function to set Morse code speed
void setMorseSpeed(MorseSpeed speed)
{
  morseSpeed = speed;

  switch (morseSpeed)
  {
  case LOW_SPEED:
    dotDuration = 500;
    dashDuration = dotDuration * 3;
    partGap = dotDuration;          // Gap between parts of the same character
    characterGap = dotDuration * 3; // Gap between characters
    break;
  case MEDIUM_SPEED:
    dotDuration = 300;
    dashDuration = dotDuration * 3;
    partGap = dotDuration;
    characterGap = dotDuration * 3;
    break;
  case HIGH_SPEED:
    dotDuration = 100;
    dashDuration = dotDuration * 3;
    partGap = dotDuration;
    characterGap = dotDuration * 3;
    break;
  }
}

// Function to load configurations from non-volatile storage
void loadConfigurations()
{
  preferences.begin("config", false);

  londonMessage = preferences.getString("londonMsg", "L");
  hilversumMessage = preferences.getString("hilversumMsg", "H");
  barcelonaMessage = preferences.getString("barcelonaMsg", "B");
  speakerDutyCycle = preferences.getUInt("volume", 64);
  morseFrequency = preferences.getUInt("frequency", 500);
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

// Wi-Fi control functions
void toggleWiFi()
{
  wifiEnabled = !wifiEnabled;

  if (wifiEnabled)
  {
    Serial.println("Turning Wi-Fi ON");
    startWiFi();
    digitalWrite(blueLEDPin, LOW); // Active-low: LOW turns LED ON
  }
  else
  {
    Serial.println("Turning Wi-Fi OFF");
    stopWiFi();
    digitalWrite(blueLEDPin, HIGH); // Active-low: HIGH turns LED OFF
  }
}

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
