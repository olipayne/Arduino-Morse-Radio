#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "Config.h"

// External variables
extern bool wifiEnabled;

// Function prototypes
void initWiFiManager(); // Initializes Wi-Fi manager
void handleWiFi();      // Handles Wi-Fi tasks
void startWiFi();       // Starts Wi-Fi and web server
void stopWiFi();        // Stops Wi-Fi and web server
void toggleWiFi();

#endif // WIFIMANAGER_H
