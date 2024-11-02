#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <Arduino.h>

void initWiFiManager();
void handleWiFi();
void startWiFi();
void stopWiFi();
void toggleWiFi();
void updateWiFiLED(); // Declare updateWiFiLED so it can be accessed in src.ino

#endif
