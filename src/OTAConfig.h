#ifndef OTA_CONFIG_H
#define OTA_CONFIG_H

#include <stddef.h>

/**
 * OTA Update Configuration
 *
 * Modify the WiFi credentials below to match your networks.
 * The OTA updater will try to connect to these networks in order
 * when performing an update.
 */

namespace OTAConfig {
// WiFi Network Credentials
// Replace these with your actual WiFi network details

struct WiFiCredential {
  const char* ssid;
  const char* password;
};

// Configure up to 3 WiFi networks
// The updater will try these in order until one connects
constexpr WiFiCredential WIFI_NETWORKS[] = {{"WillieWiFi", "Passw0rd123"},
                                            {"Reactomatic", "reactomatic1"},
                                            {"Spy Games Public", "APvern7jax"}};

constexpr size_t WIFI_NETWORK_COUNT = sizeof(WIFI_NETWORKS) / sizeof(WIFI_NETWORKS[0]);

// GitHub Repository Configuration
constexpr const char* GITHUB_OWNER = "olipayne";
constexpr const char* GITHUB_REPO = "Arduino-Morse-Radio";

// Update Timing Configuration
constexpr unsigned long BOOT_WINDOW_MS = 5000;            // 5 seconds to trigger OTA
constexpr unsigned int REQUIRED_BUTTON_PRESSES = 3;       // Number of button presses needed
constexpr unsigned long BUTTON_DEBOUNCE_MS = 200;         // Button debounce time
constexpr unsigned long WIFI_CONNECT_TIMEOUT_MS = 30000;  // 30 seconds per WiFi attempt

constexpr const char* METRICS_ENDPOINT =
    "https://arduino-morse-metrics.olbol.workers.dev/ingest";
constexpr unsigned long METRICS_SLEEP_WAKE_INTERVAL_MS = 30UL * 60UL * 1000UL;
constexpr unsigned long METRICS_HTTP_TIMEOUT_MS = 10000;

// LED Flash Configuration
constexpr unsigned long LED_FLASH_INTERVAL_MS = 500;  // LED flash rate during update
}  // namespace OTAConfig

#endif
