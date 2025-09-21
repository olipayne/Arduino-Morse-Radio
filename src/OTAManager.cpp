#include "OTAManager.h"
#include "PowerManager.h"

void OTAManager::addWiFiCredentials(const char* ssid, const char* password) {
  if (credentialCount < MAX_WIFI_CREDENTIALS) {
    wifiCredentials[credentialCount].ssid = ssid;
    wifiCredentials[credentialCount].password = password;
    credentialCount++;
  }
}

void OTAManager::clearWiFiCredentials() { credentialCount = 0; }

OTAManager::UpdateResult OTAManager::checkAndUpdate() {
#ifdef DEBUG_SERIAL_OUTPUT
  Serial.println(F("Starting OTA update check..."));
#endif

  // Step 1: Connect to WiFi with LED indication
  setLEDState(LEDState::WIFI_SEARCH);
  if (!connectToWiFi()) {
    stopLEDs();
    return UpdateResult::WIFI_FAILED;
  }

  // Step 2: Check for new version
  String latestVersion = getLatestVersion();
  if (latestVersion.isEmpty()) {
    stopLEDs();
    WiFi.disconnect();
    return UpdateResult::VERSION_CHECK_FAILED;
  }

  // Step 3: Compare versions
  String currentVersion = String(FIRMWARE_VERSION);
  if (!isNewerVersion(latestVersion, currentVersion)) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println(F("No update needed"));
#endif
    stopLEDs();
    WiFi.disconnect();
    return UpdateResult::NO_UPDATE_NEEDED;
  }

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.printf("Update available: %s -> %s\n", currentVersion.c_str(), latestVersion.c_str());
#endif

  // Step 4: Download and install
  if (downloadAndInstall(latestVersion)) {
    // If we get here, the update was successful and device will reboot
    return UpdateResult::SUCCESS;
  } else {
    stopLEDs();
    WiFi.disconnect();
    return UpdateResult::DOWNLOAD_FAILED;
  }
}

bool OTAManager::connectToWiFi() {
  if (credentialCount == 0) {
    // Add default WiFi networks if none configured
    addWiFiCredentials("YourWiFi1", "password1");
    addWiFiCredentials("YourWiFi2", "password2");
    addWiFiCredentials("YourWiFi3", "password3");
  }

  WiFi.mode(WIFI_STA);

  for (size_t i = 0; i < credentialCount; i++) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.printf("Trying to connect to %s...\n", wifiCredentials[i].ssid.c_str());
#endif

    WiFi.begin(wifiCredentials[i].ssid.c_str(), wifiCredentials[i].password.c_str());

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < WIFI_CONNECT_TIMEOUT) {
      delay(100);
    }

    if (WiFi.status() == WL_CONNECTED) {
#ifdef DEBUG_SERIAL_OUTPUT
      Serial.printf("Connected to %s\n", wifiCredentials[i].ssid.c_str());
      Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
#endif
      return true;
    }

    WiFi.disconnect();
    delay(1000);
  }

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.println(F("Failed to connect to any WiFi network"));
#endif
  return false;
}

String OTAManager::getLatestVersion() {
  HTTPClient http;
  http.begin(GITHUB_RELEASES_URL);
  http.setTimeout(HTTP_TIMEOUT);
  http.addHeader("User-Agent", "Arduino-Morse-Radio-OTA");

  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.printf("HTTP GET failed: %d\n", httpCode);
#endif
    http.end();
    return "";
  }

  String payload = http.getString();
  http.end();

  // Parse JSON response
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.printf("JSON parsing failed: %s\n", error.c_str());
#endif
    return "";
  }

  String tagName = doc["tag_name"].as<String>();

  // Remove 'v' prefix if present
  if (tagName.startsWith("v")) {
    tagName = tagName.substring(1);
  }

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.printf("Latest version: %s\n", tagName.c_str());
#endif

  return tagName;
}

bool OTAManager::downloadAndInstall(const String& version) {
  setLEDState(LEDState::DOWNLOADING);

  String downloadURL = getAssetDownloadURL(version);
  if (downloadURL.isEmpty()) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println(F("Failed to find release-ota.bin asset"));
#endif
    return false;
  }

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.printf("Downloading from: %s\n", downloadURL.c_str());
#endif

  HTTPClient http;
  http.begin(downloadURL);
  http.setTimeout(HTTP_TIMEOUT);

  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.printf("Download failed: %d\n", httpCode);
#endif
    http.end();
    return false;
  }

  int contentLength = http.getSize();
  if (contentLength <= 0) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println(F("Invalid content length"));
#endif
    http.end();
    return false;
  }

  setLEDState(LEDState::INSTALLING);

  // Begin OTA update
  if (!Update.begin(contentLength)) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.printf("OTA begin failed: %s\n", Update.errorString());
#endif
    http.end();
    return false;
  }

  WiFiClient* client = http.getStreamPtr();
  size_t written = Update.writeStream(*client);

  http.end();

  if (written != contentLength) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.printf("OTA write failed: written %d, expected %d\n", written, contentLength);
#endif
    Update.abort();
    return false;
  }

  if (!Update.end()) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.printf("OTA end failed: %s\n", Update.errorString());
#endif
    return false;
  }

  if (!Update.isFinished()) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println(F("OTA not finished"));
#endif
    return false;
  }

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.println(F("OTA update successful, rebooting..."));
#endif

  delay(1000);
  ESP.restart();

  return true;  // This line should never be reached
}

void OTAManager::setLEDState(LEDState state) {
  currentLEDState = state;
  startLEDTask(state);
}

void OTAManager::stopLEDs() {
  stopLEDTask();

  // Turn off all LEDs
  digitalWrite(Pins::LW_LED, LOW);
  digitalWrite(Pins::MW_LED, LOW);
  digitalWrite(Pins::SW_LED, LOW);
}

void OTAManager::startLEDTask(LEDState state) {
  if (ledTaskHandle != nullptr) {
    stopLEDTask();
  }

  currentLEDState = state;
  ledTaskRunning = true;

  xTaskCreate(ledTaskCode, "OTA_LED_Task", 2048, this, 1, &ledTaskHandle);
}

void OTAManager::stopLEDTask() {
  if (ledTaskHandle != nullptr) {
    ledTaskRunning = false;
    vTaskDelete(ledTaskHandle);
    ledTaskHandle = nullptr;
  }
}

void OTAManager::ledTaskCode(void* parameter) {
  OTAManager* manager = static_cast<OTAManager*>(parameter);

  int ledPin;
  switch (manager->currentLEDState) {
    case LEDState::WIFI_SEARCH:
      ledPin = Pins::SW_LED;
      break;
    case LEDState::DOWNLOADING:
      ledPin = Pins::MW_LED;
      break;
    case LEDState::INSTALLING:
      ledPin = Pins::LW_LED;
      break;
    default:
      ledPin = Pins::SW_LED;
      break;
  }

  bool ledState = false;

  while (manager->ledTaskRunning) {
    ledState = !ledState;
    digitalWrite(ledPin, ledState ? HIGH : LOW);
    vTaskDelay(pdMS_TO_TICKS(LED_FLASH_INTERVAL));
  }

  vTaskDelete(NULL);
}

bool OTAManager::isNewerVersion(const String& remoteVersion, const String& currentVersion) {
  // Simple version comparison - assumes semantic versioning (X.Y.Z)
  // This is a basic implementation - you might want to use a more robust version comparison

  int remoteMajor, remoteMinor, remotePatch;
  int currentMajor, currentMinor, currentPatch;

  // Parse remote version
  sscanf(remoteVersion.c_str(), "%d.%d.%d", &remoteMajor, &remoteMinor, &remotePatch);

  // Parse current version
  sscanf(currentVersion.c_str(), "%d.%d.%d", &currentMajor, &currentMinor, &currentPatch);

  if (remoteMajor > currentMajor) return true;
  if (remoteMajor < currentMajor) return false;

  if (remoteMinor > currentMinor) return true;
  if (remoteMinor < currentMinor) return false;

  return remotePatch > currentPatch;
}

String OTAManager::getAssetDownloadURL(const String& version) {
  HTTPClient http;
  http.begin(GITHUB_RELEASES_URL);
  http.setTimeout(HTTP_TIMEOUT);
  http.addHeader("User-Agent", "Arduino-Morse-Radio-OTA");

  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.printf("Failed to get release assets: %d\n", httpCode);
#endif
    http.end();
    return "";
  }

  String payload = http.getString();
  http.end();

  // Parse JSON response
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.printf("JSON parsing failed for assets: %s\n", error.c_str());
#endif
    return "";
  }

  // Look through assets array for release-ota.bin file
  JsonArray assets = doc["assets"];
  if (assets.isNull()) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println(F("No assets found in release"));
#endif
    return "";
  }

  for (JsonObject asset : assets) {
    String assetName = asset["name"].as<String>();

    // Look for the release-ota.bin file
    if (assetName.indexOf("release-ota.bin") >= 0) {
      String downloadURL = asset["browser_download_url"].as<String>();
#ifdef DEBUG_SERIAL_OUTPUT
      Serial.printf("Found OTA asset: %s\n", assetName.c_str());
      Serial.printf("Download URL: %s\n", downloadURL.c_str());
#endif
      return downloadURL;
    }
  }

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.println(F("No release-ota.bin asset found"));
#endif
  return "";
}

String OTAManager::buildDownloadURL(const String& version) {
  // Build URL for firmware binary (legacy method - kept for compatibility)
  // Format: https://github.com/olipayne/Arduino-Morse-Radio/releases/download/v1.11.4/firmware.bin
  String url = String(GITHUB_DOWNLOAD_BASE) + "v" + version + "/firmware.bin";
  return url;
}
