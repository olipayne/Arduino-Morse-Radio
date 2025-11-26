#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Update.h>
#include <WiFi.h>
#include "Config.h"
#include "OTAConfig.h"
#include "Version.h"

class OTAManager {
 public:
  static OTAManager& getInstance() {
    static OTAManager instance;
    return instance;
  }

  enum class UpdateResult {
    SUCCESS,
    NO_UPDATE_NEEDED,
    WIFI_FAILED,
    VERSION_CHECK_FAILED,
    DOWNLOAD_FAILED,
    INSTALL_FAILED,
    NETWORK_ERROR
  };

  enum class LEDState {
    WIFI_SEARCH,  // Long wave LED flashing (high position)
    DOWNLOADING,  // Medium wave LED flashing (middle position)
    INSTALLING    // Short wave LED flashing (low position)
  };

  // Main update process
  UpdateResult checkAndUpdate();

  // Individual steps
  bool connectToWiFi();
  String getLatestVersion();
  bool downloadAndInstall(const String& version);

  // LED control during update
  void setLEDState(LEDState state);
  void stopLEDs();

  // Configuration
  void addWiFiCredentials(const char* ssid, const char* password);
  void clearWiFiCredentials();

 private:
  OTAManager() {}
  OTAManager(const OTAManager&) = delete;
  OTAManager& operator=(const OTAManager&) = delete;

  // WiFi credentials storage
  struct WiFiCredentials {
    String ssid;
    String password;
  };

  static constexpr size_t MAX_WIFI_CREDENTIALS = 3;
  WiFiCredentials wifiCredentials[MAX_WIFI_CREDENTIALS];
  size_t credentialCount = 0;

  // GitHub API configuration
  static constexpr const char* GITHUB_RELEASES_URL =
      "https://api.github.com/repos/olipayne/Arduino-Morse-Radio/releases/latest";
  static constexpr const char* GITHUB_DOWNLOAD_BASE =
      "https://github.com/olipayne/Arduino-Morse-Radio/releases/download/";

  // Timing constants
  static constexpr unsigned long WIFI_CONNECT_TIMEOUT = 30000;  // 30 seconds
  static constexpr unsigned long HTTP_TIMEOUT = 30000;          // 30 seconds
  static constexpr unsigned long LED_FLASH_INTERVAL = 500;      // 500ms

  // LED control
  TaskHandle_t ledTaskHandle = nullptr;
  LEDState currentLEDState = LEDState::WIFI_SEARCH;
  bool ledTaskRunning = false;

  static void ledTaskCode(void* parameter);
  void startLEDTask(LEDState state);
  void stopLEDTask();

  // Helper methods
  bool isNewerVersion(const String& remoteVersion, const String& currentVersion);
  String buildDownloadURL(const String& version);
  String getAssetDownloadURL(const String& version);
  bool performOTAUpdate(const String& url);
  void flashLED(int pin, unsigned long interval);
};

#endif
