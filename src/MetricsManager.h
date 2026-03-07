#ifndef METRICS_MANAGER_H
#define METRICS_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include <esp_sleep.h>
#include <esp_system.h>

class MetricsManager {
 public:
  static MetricsManager& getInstance() {
    static MetricsManager instance;
    return instance;
  }

  void begin();
  void recordSleepEntry(uint8_t sleepReason, bool usbPowered, float batteryPercent);
  bool handleSleepWakeTelemetry();
  bool postMetricsBeforeOTAVersionCheck();
  bool maybePostPluggedInTelemetry();

 private:
  MetricsManager() = default;
  MetricsManager(const MetricsManager&) = delete;
  MetricsManager& operator=(const MetricsManager&) = delete;

  bool connectForMetrics(String& connectedOpenSSID);
  bool connectToKnownWiFi();
  bool connectToOpenWiFi(String& connectedOpenSSID);
  bool isOpenSSIDBlacklisted(const String& ssid, uint32_t nowOperationSeconds);
  void blacklistOpenSSID(const String& ssid, uint32_t nowOperationSeconds);
  static void pluggedInUploadTaskEntry(void* parameter);
  void runPluggedInUploadTask();
  bool postMetrics(const char* trigger, bool ensureWiFiConnection);
  String buildDeviceId() const;
  String wakeCauseToString(esp_sleep_wakeup_cause_t cause) const;
  String resetReasonToString(esp_reset_reason_t reason) const;
  uint32_t currentTotalOperationSeconds() const;
  void loadCounters();
  void saveCounters();

  unsigned long bootMillis = 0;
  uint32_t totalOperationSeconds = 0;
  uint32_t powerCycleCount = 0;
  uint32_t sleepCycleCount = 0;
  uint32_t telemetryPostCount = 0;
  uint32_t telemetryFailureCount = 0;
  uint32_t lastPluggedInPostOperationSeconds = 0;
  uint8_t lastSleepReason = 0;
  bool lastSleepUsbPowered = false;
  float lastSleepBatteryPercent = 0.0f;
  bool initialized = false;
  volatile bool pluggedInUploadTaskRunning = false;
  TaskHandle_t pluggedInUploadTaskHandle = nullptr;
};

#endif
