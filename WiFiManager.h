#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>

// Extern declarations
extern unsigned int speakerDutyCycle;
extern unsigned int morseFrequency;
extern String londonMessage;
extern String hilversumMessage;
extern String barcelonaMessage;
extern void saveConfigurations();
extern void setMorseSpeed(MorseSpeed speed);
extern MorseSpeed morseSpeed;

// Function prototypes
void initWiFiManager();
void handleWiFi();

#endif // WIFIMANAGER_H
