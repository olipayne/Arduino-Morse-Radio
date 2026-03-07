#include "MetricsManager.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>

#include "OTAConfig.h"
#include "PowerManager.h"
#include "Version.h"

namespace {
constexpr uint32_t kAwakeUploadMinUptimeSeconds = 3UL * 60UL;
#ifdef DEBUG_SERIAL_OUTPUT
constexpr uint32_t kAwakeUploadIntervalSeconds = 5UL * 60UL;
#else
constexpr uint32_t kAwakeUploadIntervalSeconds = 24UL * 60UL * 60UL;
#endif
constexpr uint32_t kOpenSSIDBlacklistSeconds = 7UL * 24UL * 60UL * 60UL;
constexpr size_t kMaxOpenSSIDBlacklistEntries = 8;
}

void MetricsManager::begin() {
  bootMillis = millis();
  loadCounters();
  if (esp_reset_reason() != ESP_RST_DEEPSLEEP) {
    powerCycleCount++;
  }
  initialized = true;
  saveCounters();
}

void MetricsManager::recordSleepEntry(uint8_t sleepReason, bool usbPowered, float batteryPercent) {
  if (!initialized) {
    bootMillis = millis();
    loadCounters();
    initialized = true;
  }

  totalOperationSeconds = currentTotalOperationSeconds();
  sleepCycleCount++;
  lastSleepReason = sleepReason;
  lastSleepUsbPowered = usbPowered;
  lastSleepBatteryPercent = batteryPercent;
  saveCounters();
}

bool MetricsManager::handleSleepWakeTelemetry() {
  if (OTAConfig::METRICS_ENDPOINT[0] == '\0') {
    return false;
  }

  if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_TIMER) {
    return false;
  }

  if (PowerManager::getInstance().isUSBPowered()) {
    bool posted = postMetrics("sleep_timer", true);
    if (!posted) {
      telemetryFailureCount++;
      saveCounters();
    }
  }

  PowerManager::getInstance().enterDeepSleep(PowerManager::SleepReason::INACTIVITY);
  return true;
}

bool MetricsManager::postMetricsBeforeOTAVersionCheck() {
  if (OTAConfig::METRICS_ENDPOINT[0] == '\0') {
    return false;
  }

  bool posted = postMetrics("pre_ota_check", false);
  if (!posted) {
    telemetryFailureCount++;
    saveCounters();
  }
  return posted;
}

bool MetricsManager::maybePostPluggedInTelemetry() {
  if (OTAConfig::METRICS_ENDPOINT[0] == '\0') {
    return false;
  }

  if (!PowerManager::getInstance().isUSBPowered()) {
    return false;
  }

  uint32_t nowOperationSeconds = currentTotalOperationSeconds();
  if (nowOperationSeconds < kAwakeUploadMinUptimeSeconds) {
    return false;
  }

  if (lastPluggedInPostOperationSeconds > 0 &&
      (nowOperationSeconds - lastPluggedInPostOperationSeconds) < kAwakeUploadIntervalSeconds) {
    return false;
  }

  if (pluggedInUploadTaskRunning) {
    return false;
  }

  lastPluggedInPostOperationSeconds = nowOperationSeconds;
  saveCounters();

  pluggedInUploadTaskRunning = true;
  BaseType_t taskResult = xTaskCreatePinnedToCore(MetricsManager::pluggedInUploadTaskEntry,
                                                   "metrics_usb_upload", 8192, this, 1,
                                                   &pluggedInUploadTaskHandle, 0);
  if (taskResult != pdPASS) {
    pluggedInUploadTaskRunning = false;
    pluggedInUploadTaskHandle = nullptr;
    return false;
  }

  return true;
}

void MetricsManager::pluggedInUploadTaskEntry(void* parameter) {
  MetricsManager* manager = static_cast<MetricsManager*>(parameter);
  manager->runPluggedInUploadTask();
}

void MetricsManager::runPluggedInUploadTask() {
  bool posted = postMetrics("usb_awake", true);
  if (!posted) {
    telemetryFailureCount++;
    saveCounters();
  }

  pluggedInUploadTaskRunning = false;
  pluggedInUploadTaskHandle = nullptr;
  vTaskDelete(nullptr);
}

bool MetricsManager::connectForMetrics(String& connectedOpenSSID) {
  connectedOpenSSID = "";

  if (connectToKnownWiFi()) {
    return true;
  }

  return connectToOpenWiFi(connectedOpenSSID);
}

bool MetricsManager::connectToKnownWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  WiFi.mode(WIFI_STA);
  for (size_t i = 0; i < OTAConfig::WIFI_NETWORK_COUNT; i++) {
    WiFi.begin(OTAConfig::WIFI_NETWORKS[i].ssid, OTAConfig::WIFI_NETWORKS[i].password);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED &&
           (millis() - start) < OTAConfig::WIFI_CONNECT_TIMEOUT_MS) {
      delay(100);
    }

    if (WiFi.status() == WL_CONNECTED) {
      return true;
    }

    WiFi.disconnect();
    delay(150);
  }

  WiFi.mode(WIFI_OFF);
  return false;
}

bool MetricsManager::connectToOpenWiFi(String& connectedOpenSSID) {
  WiFi.mode(WIFI_STA);
  int foundNetworks = WiFi.scanNetworks(false, true);
  if (foundNetworks <= 0) {
    WiFi.scanDelete();
    WiFi.mode(WIFI_OFF);
    return false;
  }

  uint32_t nowOperationSeconds = currentTotalOperationSeconds();
  for (int attempts = 0; attempts < foundNetworks; attempts++) {
    int bestIndex = -1;
    int bestRssi = -1000;

    for (int i = 0; i < foundNetworks; i++) {
      if (WiFi.encryptionType(i) != WIFI_AUTH_OPEN) {
        continue;
      }

      String ssid = WiFi.SSID(i);
      if (ssid.length() == 0 || isOpenSSIDBlacklisted(ssid, nowOperationSeconds)) {
        continue;
      }

      int rssi = WiFi.RSSI(i);
      if (rssi > bestRssi) {
        bestRssi = rssi;
        bestIndex = i;
      }
    }

    if (bestIndex < 0) {
      break;
    }

    String candidateSSID = WiFi.SSID(bestIndex);
    WiFi.begin(candidateSSID.c_str());
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED &&
           (millis() - start) < OTAConfig::WIFI_CONNECT_TIMEOUT_MS) {
      delay(100);
    }

    if (WiFi.status() == WL_CONNECTED) {
      connectedOpenSSID = candidateSSID;
      WiFi.scanDelete();
      return true;
    }

    blacklistOpenSSID(candidateSSID, nowOperationSeconds);
    WiFi.disconnect();
    delay(150);
  }

  WiFi.scanDelete();
  WiFi.mode(WIFI_OFF);
  return false;
}

bool MetricsManager::isOpenSSIDBlacklisted(const String& ssid, uint32_t nowOperationSeconds) {
  Preferences prefs;
  if (!prefs.begin("metrics_bl", true)) {
    return false;
  }

  bool blocked = false;
  for (size_t i = 0; i < kMaxOpenSSIDBlacklistEntries; i++) {
    char ssidKey[8];
    char untilKey[8];
    snprintf(ssidKey, sizeof(ssidKey), "bss%u", static_cast<unsigned>(i));
    snprintf(untilKey, sizeof(untilKey), "bun%u", static_cast<unsigned>(i));

    String blockedSSID = prefs.getString(ssidKey, "");
    uint32_t blockedUntil = prefs.getUInt(untilKey, 0);
    if (blockedSSID == ssid && blockedUntil > nowOperationSeconds) {
      blocked = true;
      break;
    }
  }

  prefs.end();
  return blocked;
}

void MetricsManager::blacklistOpenSSID(const String& ssid, uint32_t nowOperationSeconds) {
  Preferences prefs;
  if (!prefs.begin("metrics_bl", false)) {
    return;
  }

  uint32_t blockUntil = nowOperationSeconds + kOpenSSIDBlacklistSeconds;

  for (size_t i = 0; i < kMaxOpenSSIDBlacklistEntries; i++) {
    char ssidKey[8];
    char untilKey[8];
    snprintf(ssidKey, sizeof(ssidKey), "bss%u", static_cast<unsigned>(i));
    snprintf(untilKey, sizeof(untilKey), "bun%u", static_cast<unsigned>(i));

    String existing = prefs.getString(ssidKey, "");
    if (existing == ssid || existing.length() == 0) {
      prefs.putString(ssidKey, ssid);
      prefs.putUInt(untilKey, blockUntil);
      prefs.end();
      return;
    }
  }

  prefs.putString("bss0", ssid);
  prefs.putUInt("bun0", blockUntil);
  prefs.end();
}

bool MetricsManager::postMetrics(const char* trigger, bool ensureWiFiConnection) {
  bool connectedByManager = false;
  String connectedOpenSSID;
  if (ensureWiFiConnection && WiFi.status() != WL_CONNECTED) {
    if (!connectForMetrics(connectedOpenSSID)) {
      return false;
    }
    connectedByManager = true;
  }

  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  HTTPClient http;
  http.begin(OTAConfig::METRICS_ENDPOINT);
  http.setTimeout(OTAConfig::METRICS_HTTP_TIMEOUT_MS);
  http.addHeader("Content-Type", "application/json");

  uint32_t operationSeconds = currentTotalOperationSeconds();
  JsonDocument payload;
  payload["deviceId"] = buildDeviceId();
  payload["firmwareVersion"] = String(FIRMWARE_VERSION);
  payload["trigger"] = String(trigger);
  payload["uptimeSeconds"] = millis() / 1000UL;
  payload["totalOperationSeconds"] = operationSeconds;
  payload["totalOperationHours"] = operationSeconds / 3600.0f;
  payload["batteryVoltage"] = PowerManager::getInstance().getBatteryVoltage();
  payload["batteryPercent"] = PowerManager::getInstance().getBatteryPercent();
  payload["usbPowered"] = PowerManager::getInstance().isUSBPowered();
  payload["powerCycleCount"] = powerCycleCount;
  payload["sleepCycleCount"] = sleepCycleCount;
  payload["telemetryPostCount"] = telemetryPostCount;
  payload["telemetryFailureCount"] = telemetryFailureCount;
  payload["wakeCause"] = wakeCauseToString(esp_sleep_get_wakeup_cause());
  payload["resetReason"] = resetReasonToString(esp_reset_reason());
  payload["freeHeapBytes"] = ESP.getFreeHeap();
  payload["minFreeHeapBytes"] = ESP.getMinFreeHeap();
  payload["lastSleepReason"] = lastSleepReason;
  payload["lastSleepUsbPowered"] = lastSleepUsbPowered;
  payload["lastSleepBatteryPercent"] = lastSleepBatteryPercent;

  String body;
  serializeJson(payload, body);
  int statusCode = http.POST(body);
  http.end();

  if (connectedByManager) {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
  }

  if (statusCode < 200 || statusCode >= 300) {
    if (connectedOpenSSID.length() > 0) {
      blacklistOpenSSID(connectedOpenSSID, currentTotalOperationSeconds());
    }
    return false;
  }

  telemetryPostCount++;
  saveCounters();
  return true;
}

String MetricsManager::buildDeviceId() const {
  uint64_t chipId = ESP.getEfuseMac();
  char idBuffer[17];
  snprintf(idBuffer, sizeof(idBuffer), "%04X%08X", static_cast<uint16_t>(chipId >> 32),
           static_cast<uint32_t>(chipId));
  return String(idBuffer);
}

String MetricsManager::wakeCauseToString(esp_sleep_wakeup_cause_t cause) const {
  switch (cause) {
    case ESP_SLEEP_WAKEUP_EXT0:
      return "ext0";
    case ESP_SLEEP_WAKEUP_EXT1:
      return "ext1";
    case ESP_SLEEP_WAKEUP_TIMER:
      return "timer";
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      return "touchpad";
    case ESP_SLEEP_WAKEUP_ULP:
      return "ulp";
    default:
      return "other";
  }
}

String MetricsManager::resetReasonToString(esp_reset_reason_t reason) const {
  switch (reason) {
    case ESP_RST_POWERON:
      return "poweron";
    case ESP_RST_EXT:
      return "external";
    case ESP_RST_SW:
      return "software";
    case ESP_RST_PANIC:
      return "panic";
    case ESP_RST_INT_WDT:
      return "int_wdt";
    case ESP_RST_TASK_WDT:
      return "task_wdt";
    case ESP_RST_WDT:
      return "wdt";
    case ESP_RST_DEEPSLEEP:
      return "deepsleep";
    case ESP_RST_BROWNOUT:
      return "brownout";
    case ESP_RST_SDIO:
      return "sdio";
    default:
      return "unknown";
  }
}

uint32_t MetricsManager::currentTotalOperationSeconds() const {
  return totalOperationSeconds + ((millis() - bootMillis) / 1000UL);
}

void MetricsManager::loadCounters() {
  Preferences prefs;
  if (!prefs.begin("metrics", true)) {
    return;
  }

  totalOperationSeconds = prefs.getUInt("opSec", 0);
  powerCycleCount = prefs.getUInt("pwrCycles", 0);
  sleepCycleCount = prefs.getUInt("slpCycles", 0);
  telemetryPostCount = prefs.getUInt("posts", 0);
  telemetryFailureCount = prefs.getUInt("postFail", 0);
  lastPluggedInPostOperationSeconds = prefs.getUInt("lastPiw", 0);
  lastSleepReason = static_cast<uint8_t>(prefs.getUChar("slpReason", 0));
  lastSleepUsbPowered = prefs.getBool("slpUsb", false);
  lastSleepBatteryPercent = prefs.getFloat("slpBatt", 0.0f);
  prefs.end();
}

void MetricsManager::saveCounters() {
  Preferences prefs;
  if (!prefs.begin("metrics", false)) {
    return;
  }

  prefs.putUInt("opSec", totalOperationSeconds);
  prefs.putUInt("pwrCycles", powerCycleCount);
  prefs.putUInt("slpCycles", sleepCycleCount);
  prefs.putUInt("posts", telemetryPostCount);
  prefs.putUInt("postFail", telemetryFailureCount);
  prefs.putUInt("lastPiw", lastPluggedInPostOperationSeconds);
  prefs.putUChar("slpReason", lastSleepReason);
  prefs.putBool("slpUsb", lastSleepUsbPowered);
  prefs.putFloat("slpBatt", lastSleepBatteryPercent);
  prefs.end();
}
