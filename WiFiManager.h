#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include "Config.h"  // Include the Config.h file for shared definitions

// Extern variables
extern bool wifiEnabled;

// Function prototypes
void initWiFiManager();
void handleWiFi();
void startWiFi();
void stopWiFi();

#endif // WIFIMANAGER_H
