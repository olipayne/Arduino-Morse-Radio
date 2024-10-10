#include <Arduino.h>
#include "Config.h"
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

// Wi-Fi toggle button
const int wifiButtonPin = D2;          // GPIO pin connected to the Wi-Fi toggle button
bool wifiEnabled = false;              // Wi-Fi is off by default
unsigned long lastButtonPress = 0;     // For debouncing

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

  // Configure the Wi-Fi button pin
  pinMode(wifiButtonPin, INPUT_PULLUP);  // Enable internal pull-up resistor

  // Initialize Wi-Fi (Wi-Fi is off by default)
  initWiFiManager();  // Initialize Wi-Fi Manager
}

void loop() {
  // Read the potentiometer value (0-4095 for ESP32)
  int potValue = analogRead(potentiometerPin);
