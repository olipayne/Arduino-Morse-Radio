#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "Config.h"
#include "StationManager.h"

class WiFiManager
{
public:
    static WiFiManager &getInstance()
    {
        static WiFiManager instance;
        return instance;
    }

    // Core functionality
    void begin();
    void handle();
    void toggle();

    // Status and control
    bool isEnabled() const { return wifiEnabled; }
    void stop();
    void updateStatusLED();

    // Configuration
    void setTimeoutDuration(unsigned long duration) { timeoutDuration = duration; }
    void setHostname(const String &name);

private:
    WiFiManager();
    WiFiManager(const WiFiManager &) = delete;
    WiFiManager &operator=(const WiFiManager &) = delete;

    // Web server handlers
    void setupServer();
    void handleRoot();
    void handleSaveConfig();
    void handleStationConfig();
    void handleAPI();
    void handleNotFound();

    // HTML generation
    String generateHTML(const String &content) const;
    String generateConfigPage() const;
    String generateStationTable() const;
    String generateStatusJson() const;

    // WiFi setup
    void startAP();
    void setupMDNS();

    // LED control
    void flashLED();

    // Internal state
    WebServer server;
    bool wifiEnabled;
    unsigned long startTime;
    unsigned long lastLedFlash;
    unsigned long timeoutDuration;
    String hostname;

    // Constants
    static constexpr unsigned long DEFAULT_TIMEOUT = 120000; // 2 minutes
    static constexpr unsigned long LED_FLASH_INTERVAL = 500;
    static constexpr uint8_t AP_CHANNEL = 1;
    static constexpr uint8_t MAX_CONNECTIONS = 4;

    // HTML templates
    static const char *HTML_HEADER;
    static const char *HTML_FOOTER;
    static const char *CSS_STYLES;
};

#endif