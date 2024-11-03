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

    void begin();
    void handle();
    void toggle();
    void stop();
    bool isEnabled() const { return wifiEnabled; }
    void updateStatusLED();

private:
    WiFiManager() : server(80),
                    wifiEnabled(false),
                    startTime(0),
                    lastLedFlash(0),
                    hostname("radio-config") {}

    WiFiManager(const WiFiManager &) = delete;
    WiFiManager &operator=(const WiFiManager &) = delete;

    void setupServer();
    void startAP();
    void setupMDNS();
    void flashLED();

    // Web handlers
    void handleRoot();
    void handleSaveConfig();
    void handleGetTuningValue();
    void handleStationConfig();
    void handleAPI();
    void handleNotFound();

    // HTML generation
    String generateHTML(const String &content) const;
    String generateConfigPage() const;
    String generateStationTable() const;
    String generateStatusJson() const;

    // Server instance
    WebServer server;
    bool wifiEnabled;
    unsigned long startTime;
    unsigned long lastLedFlash;
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
    static const char *JAVASCRIPT_CODE;
};

#endif