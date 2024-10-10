#include <Arduino.h>
#include "Stations.h"
#include "MorseCode.h"
#include "WiFiManager.h"

// Define the potentiometer and other pins
const int potentiometerPin = A0;       // Potentiometer pin
const int ledLockPin = LED_BUILTIN;    // LED_BUILTIN to indicate a lock
const int speakerPin = D7;             // Speaker pin for Morse code output

// Define PWM channel, frequency, and resolution
const int pwmFrequency = 5000;
const int pwmResolution = 8;           // 8-bit resolution (0-255)
const int lockChannel = 3;             // PWM channel for the lock LED
const int speakerChannel = 4;          // PWM channel for the speaker

// Define leeway
const int leeway = 100;                // Adjust as needed for the range around each station

// Variables for speaker adjustments (loaded from configurations)
unsigned int speakerDutyCycle = 64;    // Speaker volume (duty cycle)
unsigned int morseFrequency = 500;     // Speaker frequency (Hz)

// Variables to keep track of the Morse code playback
String currentMorseMessage = "";
int currentMorseIndex = 0;
String currentMorseCode = "";
int currentMorseCodeIndex = 0;
unsigned long morseTimer = 0;
bool morsePlaying = false;
bool morseToneOn = false;

// Morse code speed settings
enum MorseSpeed { LOW_SPEED, MEDIUM_SPEED, HIGH_SPEED };
MorseSpeed morseSpeed = MEDIUM_SPEED;  // Default speed

// Timing variables (in milliseconds)
unsigned int dotDuration = 100;         // Duration of a dot
unsigned int dashDuration = 300;        // Duration of a dash
unsigned int partGap = 100;             // Gap between parts of the same character
unsigned int characterGap = 500;        // Gap between characters

// Configuration variables
String londonMessage = "L";
String hilversumMessage = "H";
String barcelonaMessage = "B";

// Preferences for storing configurations
Preferences preferences;

void setup() {
  // Start the serial communication at 115200 baud rate
  Serial.begin(115200);

  // Configure the LED PWM channels for the stations
  for (int i = 0; i < numStations; i++) {
    ledcSetup(stations[i].ledChannel, pwmFrequency, pwmResolution);
    ledcAttachPin(stations[i].ledPin, stations[i].ledChannel);
  }

  // Configure the lock LED PWM channel
  ledcSetup(lockChannel, pwmFrequency, pwmResolution);
  ledcAttachPin(ledLockPin, lockChannel);

  // Configure the speaker PWM channel
  ledcSetup(speakerChannel, pwmFrequency, pwmResolution);
  ledcAttachPin(speakerPin, speakerChannel);

  // Initially set the speaker duty cycle to zero (silent)
  ledcWrite(speakerChannel, 0);

  // Load configurations from NVS
  loadConfigurations();

  // Set initial Morse code speed
  setMorseSpeed(morseSpeed);

  // Initialize Wi-Fi (Wi-Fi logic is in WiFiManager)
  initWiFiManager();
}

void loop() {
  // Read the potentiometer value (0-4095 for ESP32)
  int potValue = analogRead(potentiometerPin);

  unsigned long currentTime = millis();
  bool stationLocked = false;
  int lockBrightness = 0;
  Station* lockedStation = nullptr;

  // Turn off all station LEDs
  for (int i = 0; i < numStations; i++) {
    ledcWrite(stations[i].ledChannel, 0);
  }

  // Determine which station is closest
  for (int i = 0; i < numStations; i++) {
    int brightness = calculateBrightness(potValue, stations[i].frequency);
    if (brightness > 0) {
      // Turn on this station's LED
      ledcWrite(stations[i].ledChannel, brightness);

      // Check if we are locked onto this station
      if (abs(potValue - stations[i].frequency) <= leeway) {
        lockBrightness = brightness;
        stationLocked = true;
        lockedStation = &stations[i];
      }
      // Since we only want one LED on at a time, break after the first one found
      break;
    }
  }

  // Set the lock LED brightness
  ledcWrite(lockChannel, lockBrightness);

  if (stationLocked) {
    if (!morsePlaying) {
      // Set currentMorseMessage based on station name
      if (strcmp(lockedStation->name, "London") == 0) {
        currentMorseMessage = londonMessage;
      } else if (strcmp(lockedStation->name, "Hilversum") == 0) {
        currentMorseMessage = hilversumMessage;
      } else if (strcmp(lockedStation->name, "Barcelona") == 0) {
        currentMorseMessage = barcelonaMessage;
      } else {
        currentMorseMessage = "S";  // Default message
      }

      currentMorseIndex = 0;
      currentMorseCodeIndex = 0;
      currentMorseCode = getMorseCode(currentMorseMessage[currentMorseIndex]); // Initialize currentMorseCode
      morsePlaying = true;
      morseTimer = currentTime;
      morseToneOn = false;
    }
  } else {
    // No station locked
    morsePlaying = false;
    ledcWrite(speakerChannel, 0); // Stop any tone by setting duty cycle to zero
  }

  // Handle Morse code playing
  if (morsePlaying) {
    if (currentTime >= morseTimer) {
      if (morseToneOn) {
        // Turn off the tone
        ledcWrite(speakerChannel, 0); // Stop the tone
        morseToneOn = false;
        // Set the next interval (gap between parts of the same character)
        morseTimer = currentTime + partGap;
      } else {
        if (currentMorseCodeIndex < currentMorseCode.length()) {
          // Play the next dot or dash
          char symbol = currentMorseCode[currentMorseCodeIndex];
          if (symbol == '.') {
            // Play dot
            ledcWriteTone(speakerChannel, morseFrequency);
            ledcWrite(speakerChannel, speakerDutyCycle);
            morseToneOn = true;
            morseTimer = currentTime + dotDuration;
          } else if (symbol == '-') {
            // Play dash
            ledcWriteTone(speakerChannel, morseFrequency);
            ledcWrite(speakerChannel, speakerDutyCycle);
            morseToneOn = true;
            morseTimer = currentTime + dashDuration;
          }
          currentMorseCodeIndex++;
        } else {
          // Finished current character, move to next character
          currentMorseIndex++;
          if (currentMorseIndex < currentMorseMessage.length()) {
            // Get Morse code for the next character
            char nextChar = currentMorseMessage[currentMorseIndex];
            currentMorseCode = getMorseCode(nextChar);
            currentMorseCodeIndex = 0;
            // Set gap between characters
            morseTimer = currentTime + characterGap;
          } else {
            // Finished the message
            morsePlaying = false;
            ledcWrite(speakerChannel, 0); // Ensure tone is stopped
          }
        }
      }
    }
  }

  // Handle Wi-Fi tasks
  handleWiFi();

  // Add a short delay for stability
  delay(10);  // Adjust delay as needed (10 ms)
}

// Function to calculate LED brightness based on proximity to a target value
int calculateBrightness(int potValue, int targetValue) {
  int difference = abs(potValue - targetValue);

  if (difference <= leeway) {
    return map(difference, 0, leeway, 255, 0);
  } else {
    return 0;
  }
}

// Function to set Morse code speed
void setMorseSpeed(MorseSpeed speed) {
  morseSpeed = speed;

  switch (morseSpeed) {
    case LOW_SPEED:
      dotDuration = 200;
      dashDuration = 600;
      partGap = 200;
      characterGap = 1000;
      break;
    case MEDIUM_SPEED:
      dotDuration = 100;
      dashDuration = 300;
      partGap = 100;
      characterGap = 500;
      break;
    case HIGH_SPEED:
      dotDuration = 50;
      dashDuration = 150;
      partGap = 50;
      characterGap = 250;
      break;
  }
}

// Preferences Functions
void loadConfigurations() {
  preferences.begin("config", false);

  londonMessage = preferences.getString("londonMsg", "L");
  hilversumMessage = preferences.getString("hilversumMsg", "H");
  barcelonaMessage = preferences.getString("barcelonaMsg", "B");
  speakerDutyCycle = preferences.getUInt("volume", 64);       // Default volume
  morseFrequency = preferences.getUInt("frequency", 500);     // Default frequency
  morseSpeed = static_cast<MorseSpeed>(preferences.getUInt("morseSpeed", MEDIUM_SPEED)); // Default speed

  preferences.end();

  // Update Morse code timing
  setMorseSpeed(morseSpeed);
}

void saveConfigurations() {
  preferences.begin("config", false);

  preferences.putString("londonMsg", londonMessage);
  preferences.putString("hilversumMsg", hilversumMessage);
  preferences.putString("barcelonaMsg", barcelonaMessage);
  preferences.putUInt("volume", speakerDutyCycle);
  preferences.putUInt("frequency", morseFrequency);
  preferences.putUInt("morseSpeed", morseSpeed);

  preferences.end();
}
