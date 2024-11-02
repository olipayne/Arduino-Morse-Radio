#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <Arduino.h>
#include <WebServer.h>

namespace WiFiManager
{
    // Function prototypes
    void initWiFiManager(); // Initializes Wi-Fi manager
    void handleWiFi();      // Handles Wi-Fi tasks
    void startWiFi();       // Starts Wi-Fi and web server
    void stopWiFi();        // Stops Wi-Fi and web server
    void toggleWiFi();

    extern bool wifiEnabled;            // Wi-Fi state
    extern unsigned long wifiStartTime; // Wi-Fi start time
} // namespace WiFiManager

#endif // WIFIMANAGER_H
