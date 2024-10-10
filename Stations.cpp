#include "Stations.h"

// Define PWM channels for LEDs
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;

// Define LED pins
const int ledRedPin = LED_RED;       // Adjust according to your wiring
const int ledGreenPin = LED_GREEN;   // Adjust according to your wiring
const int ledBluePin = LED_BLUE;     // Adjust according to your wiring

// Define the number of stations
const int numStations = 3;

// Initialize the stations array
Station stations[] = {
  {"London",     1000, redChannel,   ledRedPin},
  {"Hilversum",  2000, greenChannel, ledGreenPin},
  {"Barcelona",  3000, blueChannel,  ledBluePin}
};
