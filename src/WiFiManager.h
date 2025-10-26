#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <WiFi.h>
#include "AudioManager.h"
#include "Config.h"
#include "MorseCode.h"
#include "PowerManager.h"
#include "StationManager.h"
#include "Version.h"  // Include version information
#include "esp_timer.h"

class WiFiManager {
 public:
  static WiFiManager& getInstance() {
    static WiFiManager instance;
    return instance;
  }

  void begin();
  void handle();
  void toggle();
  void stop();
  bool isEnabled() const { return wifiEnabled; }
  void updateStatusLED();
  bool hasConnectedClients() const { return WiFi.softAPgetStationNum() > 0; }

 private:
  WiFiManager()
      : server(80),
        wifiEnabled(false),
        startTime(0),
        lastLedFlash(0),
        hostname("radio-config"),
        timer(nullptr) {}

  WiFiManager(const WiFiManager&) = delete;
  WiFiManager& operator=(const WiFiManager&) = delete;

  void setupServer();
  void startAP();
  void setupMDNS();
  void flashLED();

  // Web handlers
  void handleRoot();
  void handleStationConfig();
  void handleCalibration();
  void handleSaveConfig();
  void handleSaveFrequency();
  void handleGetTuningValue();
  void handleAPI();
  void handleNotFound();
  void handleExportMessages();
  void handleImportMessages();

  // HTML generation
  String generateHTML(const String& content) const;
  String generateHomePage() const;
  String generateStationPage() const;
  String generateStationList() const;
  String generateCalibrationPage() const;
  String generateStatusJson() const;

  // Server instance
  WebServer server;
  bool wifiEnabled;
  unsigned long startTime;
  unsigned long lastLedFlash;
  String hostname;
  esp_timer_handle_t timer;  // Timer handle for OTA operations

  // Constants
  static constexpr unsigned long DEFAULT_TIMEOUT = 120000;  // 2 minutes
  static constexpr unsigned long LED_FLASH_INTERVAL = 500;
  static constexpr uint8_t AP_CHANNEL = 1;
  static constexpr uint8_t MAX_CONNECTIONS = 4;

  // HTML templates stored in PROGMEM
  static const char HTML_HEADER[];
  static const char HTML_FOOTER[];
  static const char CSS_STYLES[];
  static const char JAVASCRIPT_CODE[];
};

#endif
