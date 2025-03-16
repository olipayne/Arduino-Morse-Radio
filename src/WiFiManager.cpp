#include "WiFiManager.h"
#include <ArduinoJson.h>
#include <ElegantOTA.h>
#include "Version.h"  // Include the auto-generated version header

// Define static members
const char* WiFiManager::HTML_HEADER = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <title>Radio Configuration</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta name="theme-color" content="#2196F3">
    <style>)";

const char* WiFiManager::CSS_STYLES = R"(
    :root {
        --primary-color: #2196F3;
        --success-color: #4CAF50;
        --error-color: #f44336;
        --background-color: #f5f5f5;
        --card-background: #ffffff;
        --text-color: #333333;
        --border-color: #e0e0e0;
        --spacing-unit: 16px;
        --border-radius: 8px;
        
        /* Wave band colors - matching LED colors */
        --long-wave-color: #e53935;  /* Warm red */
        --medium-wave-color: #fdd835; /* Bright yellow */
        --short-wave-color: #1e88e5;  /* Clear blue */
        
        /* Wave band background colors - soft tints of the LED colors */
        --long-wave-bg: #ffebee;    /* Very light red */
        --medium-wave-bg: #fffde7;  /* Very light yellow */
        --short-wave-bg: #e3f2fd;   /* Very light blue */
    }

    @media (prefers-color-scheme: dark) {
        :root {
            --background-color: #121212;
            --card-background: #1e1e1e;
            --text-color: #ffffff;
            --border-color: #333333;
            
            /* Dark mode wave band background colors */
            --long-wave-bg: rgba(229, 57, 53, 0.15);    /* Dark red */
            --medium-wave-bg: rgba(253, 216, 53, 0.15); /* Dark yellow */
            --short-wave-bg: rgba(30, 136, 229, 0.15);  /* Dark blue */
        }
    }

    * {
        box-sizing: border-box;
        margin: 0;
        padding: 0;
    }

    body { 
        font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
        line-height: 1.5;
        max-width: 800px; 
        margin: 0 auto; 
        padding: var(--spacing-unit);
        background-color: var(--background-color);
        color: var(--text-color);
        padding-bottom: calc(var(--spacing-unit) * 5);
        padding-top: calc(var(--spacing-unit) * 4);
    }

    .station { 
        background: var(--card-background);
        padding: var(--spacing-unit);
        margin: var(--spacing-unit) 0;
        border-radius: var(--border-radius);
        box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        border: 1px solid var(--border-color);
    }

    .station-header {
        display: flex;
        flex-wrap: wrap;
        gap: var(--spacing-unit);
        align-items: center;
        margin-bottom: var(--spacing-unit);
    }

    .station-name {
        flex: 1;
        font-size: 1.2em;
        font-weight: 600;
    }

    .input-group {
        display: flex;
        gap: 8px;
        margin-bottom: var(--spacing-unit);
    }

    input, button {
        font-size: 16px; /* Prevents iOS zoom on focus */
        border-radius: var(--border-radius);
        padding: 12px;
        border: 1px solid var(--border-color);
        background: var(--card-background);
        color: var(--text-color);
    }

    input[type="checkbox"] {
        width: 24px;
        height: 24px;
        margin: 0;
        accent-color: var(--primary-color);
    }

    input[type="number"] {
        width: 120px;
        -moz-appearance: textfield;
    }

    input[type="number"]::-webkit-outer-spin-button,
    input[type="number"]::-webkit-inner-spin-button {
        -webkit-appearance: none;
    }

    input[type="text"] {
        width: 100%;
    }

    .enable-toggle {
        display: flex;
        align-items: center;
        gap: 12px;
    }

    .enable-toggle label {
        font-size: 1em;
        margin: 0;
    }

    button { 
        background: var(--primary-color);
        color: white;
        border: none;
        font-weight: 500;
        cursor: pointer;
        transition: opacity 0.2s, transform 0.1s;
        min-height: 44px;
        padding: 0 24px;
    }

    button:hover { 
        opacity: 0.9;
    }

    button:active {
        transform: scale(0.98);
    }

    .tune-button {
        white-space: nowrap;
        min-width: 100px;
    }

    .wave-band { 
        padding: var(--spacing-unit);
        margin: var(--spacing-unit) 0;
        border-radius: var(--border-radius);
        background: var(--card-background);
        border: 1px solid var(--border-color);
        position: relative;
        overflow: hidden;
    }

    .wave-band::before {
        content: '';
        position: absolute;
        left: 0;
        top: 0;
        bottom: 0;
        width: 4px;
    }

    .wave-band h2 {
        display: flex;
        align-items: center;
        gap: var(--spacing-unit);
        margin-bottom: var(--spacing-unit);
        padding-bottom: var(--spacing-unit);
        border-bottom: 1px solid var(--border-color);
    }

    /* Wave band specific styles */
    .wave-band.long-wave {
        background: var(--long-wave-bg);
    }
    .wave-band.long-wave::before {
        background: var(--long-wave-color);
    }
    .wave-band.long-wave h2 {
        color: var(--long-wave-color);
    }

    .wave-band.medium-wave {
        background: var(--medium-wave-bg);
    }
    .wave-band.medium-wave::before {
        background: var(--medium-wave-color);
    }
    .wave-band.medium-wave h2 {
        color: var(--medium-wave-color);
    }

    .wave-band.short-wave {
        background: var(--short-wave-bg);
    }
    .wave-band.short-wave::before {
        background: var(--short-wave-color);
    }
    .wave-band.short-wave h2 {
        color: var(--short-wave-color);
    }

    /* Station cards within wave bands */
    .wave-band .station {
        background: var(--card-background);
        border-color: var(--border-color);
    }

    .status { 
        position: fixed;
        top: 0;
        left: 0;
        right: 0;
        background: var(--card-background);
        padding: var(--spacing-unit);
        box-shadow: 0 2px 8px rgba(0,0,0,0.1);
        text-align: center;
        z-index: 1000;
        font-weight: 500;
        color: var(--primary-color);
        backdrop-filter: blur(8px);
    }

    .save-button-container {
        position: fixed;
        bottom: 0;
        left: 0;
        right: 0;
        background: var(--card-background);
        padding: var(--spacing-unit);
        box-shadow: 0 -2px 8px rgba(0,0,0,0.1);
        z-index: 1000;
        backdrop-filter: blur(8px);
    }

    .save-button-container button {
        width: 100%;
        max-width: 800px;
        margin: 0 auto;
        background: var(--success-color);
    }

    .save-button-container button:disabled {
        opacity: 0.5;
        cursor: not-allowed;
    }

    .save-button-container button.saving {
        position: relative;
        color: transparent;
    }

    .save-button-container button.saving::after {
        content: 'Saving...';
        position: absolute;
        left: 50%;
        top: 50%;
        transform: translate(-50%, -50%);
        color: white;
    }

    .toast {
        position: fixed;
        top: calc(var(--spacing-unit) * 4);
        left: 50%;
        transform: translateX(-50%);
        background: var(--success-color);
        color: white;
        padding: 12px 24px;
        border-radius: var(--border-radius);
        box-shadow: 0 4px 12px rgba(0,0,0,0.15);
        z-index: 2000;
        font-weight: 500;
        opacity: 0;
        transition: 0.3s ease-in-out;
        pointer-events: none;
    }

    .toast.error {
        background: var(--error-color);
    }

    .toast.show {
        opacity: 1;
    }

    h1 {
        color: var(--text-color);
        border-bottom: 2px solid var(--primary-color);
        padding-bottom: var(--spacing-unit);
        margin: 0 0 var(--spacing-unit) 0;
        font-size: 1.5em;
        font-weight: 600;
    }

    h2 {
        color: var(--text-color);
        margin: 0 0 var(--spacing-unit) 0;
        font-size: 1.3em;
        font-weight: 600;
    }

    @media (max-width: 600px) {
        :root {
            --spacing-unit: 12px;
        }

        body {
            padding: 12px;
            padding-bottom: calc(var(--spacing-unit) * 5);
            padding-top: calc(var(--spacing-unit) * 4);
        }

        .station-header {
            flex-direction: column;
            align-items: flex-start;
            gap: 8px;
        }

        .input-group {
            flex-direction: column;
        }

        .tune-button {
            width: 100%;
        }

        input[type="number"] {
            width: 100%;
        }

        .enable-toggle {
            width: 100%;
            justify-content: space-between;
        }
    }

    @media (hover: none) {
        button:hover {
            opacity: 1;
        }
    }

    /* Improved focus styles for accessibility */
    :focus {
        outline: 2px solid var(--primary-color);
        outline-offset: 2px;
    }

    :focus:not(:focus-visible) {
        outline: none;
    }

    /* Loading indicator for the save button */
    @keyframes saving-pulse {
        0% { opacity: 1; }
        50% { opacity: 0.5; }
        100% { opacity: 1; }
    }

    .saving {
        animation: saving-pulse 1.5s infinite;
    }
)";

const char* WiFiManager::HTML_FOOTER = R"(
    </body>
</html>)";

const char* WiFiManager::JAVASCRIPT_CODE = R"(
    function updateTuningValue() {
        fetch('/tuning')
            .then(response => response.json())
            .then(data => {
                document.getElementById('currentTuning').textContent = data.value;
            })
            .catch(error => console.error('Error fetching tuning value:', error));
    }

    function setFrequency(inputId) {
        fetch('/tuning')
            .then(response => response.json())
            .then(data => {
                const input = document.getElementById(inputId);
                input.value = data.value;
                input.style.backgroundColor = '#e8f5e9';
                setTimeout(() => input.style.backgroundColor = '', 500);
            })
            .catch(error => console.error('Error setting frequency:', error));
    }

    function showToast(message, isError = false) {
        const toast = document.createElement('div');
        toast.className = 'toast' + (isError ? ' error' : '');
        toast.textContent = message;
        document.body.appendChild(toast);
        
        // Trigger reflow to ensure animation works
        toast.offsetHeight;
        toast.classList.add('show');
        
        setTimeout(() => {
            toast.classList.remove('show');
            setTimeout(() => toast.remove(), 300);
        }, 3000);
    }

    function handleFormSubmit(event) {
        event.preventDefault();
        const form = event.target;
        const submitButton = form.querySelector('button[type="submit"]');
        
        // Disable button and show saving state
        submitButton.disabled = true;
        submitButton.classList.add('saving');
        
        // Collect form data into a structured object
        const stations = [];
        const stationData = {};
        
        // First pass: collect all inputs
        form.querySelectorAll('input').forEach(input => {
            const match = input.name.match(/(freq|msg|enable)_(\d+)/);
            if (match) {
                const [, type, index] = match;
                if (!stationData[index]) {
                    stationData[index] = { index: parseInt(index) };
                }
                
                if (type === 'freq') {
                    stationData[index].frequency = parseInt(input.value);
                } else if (type === 'msg') {
                    stationData[index].message = input.value;
                } else if (type === 'enable') {
                    stationData[index].enabled = input.checked;
                }
            }
        });
        
        // Convert to array and ensure enabled is set
        for (const index in stationData) {
            const station = stationData[index];
            if (!('enabled' in station)) {
                station.enabled = false;
            }
            stations.push(station);
        }

        // Send as JSON
        fetch('/save', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ stations })
        })
        .then(response => response.json())
        .then(data => {
            showToast(data.message, !data.success);
        })
        .catch(error => {
            console.error('Error:', error);
            showToast('Error saving configuration', true);
        })
        .finally(() => {
            // Re-enable button and remove saving state
            submitButton.disabled = false;
            submitButton.classList.remove('saving');
        });
    }

    // Update tuning value every 500ms
    setInterval(updateTuningValue, 500);

    // Add form submit handler
    document.addEventListener('DOMContentLoaded', () => {
        const form = document.querySelector('form');
        if (form) {
            form.addEventListener('submit', handleFormSubmit);
        }
    });
)";

void WiFiManager::begin() {
  pinMode(Pins::WIFI_BUTTON, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Initialize timer for system tasks
  esp_timer_create_args_t timer_config = {.callback = nullptr,
                                          .arg = nullptr,
                                          .dispatch_method = ESP_TIMER_TASK,
                                          .name = "system_timer",
                                          .skip_unhandled_events = true};
  esp_timer_create(&timer_config, &timer);
}

void WiFiManager::toggle() {
  if (wifiEnabled) {
    stop();
  } else {
    startAP();
  }
}

void WiFiManager::stop() {
  if (wifiEnabled) {
    server.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    wifiEnabled = false;
    digitalWrite(LED_BUILTIN, LOW);
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("WiFi stopped");
#endif
  }
}

void WiFiManager::startAP() {
  WiFi.mode(WIFI_AP);

  String ssid = "Radio_" + String((uint32_t)ESP.getEfuseMac(), HEX);

  if (WiFi.softAP(ssid.c_str(), nullptr, AP_CHANNEL, false, MAX_CONNECTIONS)) {
    setupServer();
    setupMDNS();

    wifiEnabled = true;
    startTime = millis();

#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("WiFi AP started: " + ssid);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("Free heap: ");
    Serial.println(ESP.getFreeHeap());
    Serial.print("Flash size: ");
    Serial.println(ESP.getFlashChipSize());
    Serial.print("Sketch size: ");
    Serial.println(ESP.getSketchSize());
    Serial.print("Free sketch space: ");
    Serial.println(ESP.getFreeSketchSpace());
#endif
  } else {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("Failed to start WiFi AP");
#endif
  }
}

void WiFiManager::handle() {
  if (!wifiEnabled) return;

  server.handleClient();
  ElegantOTA.loop();

  // Only check timeout when running on battery power
  if (!PowerManager::getInstance().isUSBPowered() && millis() - startTime > DEFAULT_TIMEOUT) {
    stop();
    return;
  }

#ifdef DEBUG_SERIAL_OUTPUT
  static unsigned long lastStatusCheck = 0;
  if (millis() - lastStatusCheck > 5000) {  // Check every 5 seconds
    Serial.printf("Connected stations: %d\n", WiFi.softAPgetStationNum());
    Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
    lastStatusCheck = millis();
  }
#endif

  updateStatusLED();
}

void WiFiManager::setupServer() {
  server.on("/", HTTP_GET, [this]() { handleRoot(); });
  server.on("/save", HTTP_POST, [this]() { handleSaveConfig(); });
  server.on("/tuning", HTTP_GET, [this]() { handleGetTuningValue(); });
  server.on("/stations", HTTP_GET, [this]() { handleStationConfig(); });
  server.on("/api/status", HTTP_GET, [this]() { handleAPI(); });
  server.onNotFound([this]() { handleNotFound(); });

  // Add OTA event handlers
  ElegantOTA.onStart([this]() {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("OTA Update Started");
#endif
    // Stop all tasks that might interfere with the update
    PowerManager::getInstance().stopLEDTask();
    StationManager::getInstance().saveToPreferences();  // Save current state

    // Stop audio and other tasks
    AudioManager::getInstance().stop();
    MorseCode::getInstance().stop();

    // Disable timer if it's running
    if (timer != nullptr) {
      esp_timer_stop(timer);
    }

    // Free up memory
    ESP.getMinFreeHeap();
  });

  // Add a custom warning to the ElegantOTA interface
  ElegantOTA.setAutoReboot(true);
  ElegantOTA.begin(&server);

  server.begin();
}

void WiFiManager::setupMDNS() {
  if (MDNS.begin(hostname.c_str())) {
    MDNS.addService("http", "tcp", 80);
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("MDNS responder started at http://" + hostname + ".local");
#endif
  }
}

void WiFiManager::handleRoot() {
  server.send(200, "text/html", generateHTML(generateConfigPage()));
}

void WiFiManager::handleGetTuningValue() {
  int tuningValue = PowerManager::getInstance().readADC(Pins::TUNING_POT);
  String response = "{\"value\":" + String(tuningValue) + "}";
  server.send(200, "application/json", response);
  startTime = millis();  // Reset the timeout counter
}

void WiFiManager::handleSaveConfig() {
  bool success = true;
  String message = "Configuration saved successfully";

  // Get the raw POST data
  String jsonData = server.arg("plain");

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.println("Received JSON data: " + jsonData);
#endif

  // Parse JSON using ArduinoJson
  JsonDocument doc;  // No capacity needed in v7
  DeserializationError error = deserializeJson(doc, jsonData);

  if (error) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("Failed to parse JSON: ");
    Serial.println(error.c_str());
#endif
    server.send(400, "application/json", "{\"success\":false,\"message\":\"Invalid JSON format\"}");
    return;
  }

  // Get the stations array
  JsonArray stations = doc["stations"];
  if (stations.isNull()) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("No stations array in JSON");
#endif
    server.send(400, "application/json", "{\"success\":false,\"message\":\"Invalid JSON format\"}");
    return;
  }

  auto& stationManager = StationManager::getInstance();

  // Process each station
  for (JsonObject station : stations) {
    int index = station["index"].as<int>();
    int frequency = station["frequency"].as<int>();
    const char* stationMessage = station["message"].as<const char*>();
    bool enabled = station["enabled"].as<bool>();

#ifdef DEBUG_SERIAL_OUTPUT
    Serial.printf("Processing station - Index: %d, Frequency: %d, Message: %s, Enabled: %s\n",
                  index, frequency, stationMessage, enabled ? "true" : "false");
#endif

    if (frequency > 0) {
      stationManager.updateStation(index, frequency, stationMessage, enabled);
#ifdef DEBUG_SERIAL_OUTPUT
      Serial.printf("Updated station %d\n", index);
#endif
    } else {
#ifdef DEBUG_SERIAL_OUTPUT
      Serial.printf("Invalid frequency value for station %d: %d\n", index, frequency);
#endif
      success = false;
      message = "Invalid frequency value for station " + String(index);
      break;
    }
  }

  if (success) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("Saving to preferences...");
#endif
    stationManager.saveToPreferences();
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("Successfully saved to preferences");
#endif
  }

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.printf("Save result - Success: %s, Message: %s\n", success ? "true" : "false",
                message.c_str());
#endif

  // Create JSON response
  JsonDocument response;  // No capacity needed in v7
  response["success"] = success;
  response["message"] = message;

  String responseStr;
  serializeJson(response, responseStr);
  server.send(200, "application/json", responseStr);
}

String WiFiManager::generateConfigPage() const {
  String html =
      "<div class='status' role='status' aria-live='polite'>Current Tuning Value: <span "
      "id='currentTuning'>-</span> | Firmware: " +
      String(FIRMWARE_VERSION) + "</div>";
  html += "<h1>Radio Configuration</h1>";
  html +=
      "<form method='POST' action='/save' id='configForm' aria-label='Radio station configuration "
      "form'>";
  html += generateStationTable();
  html += "<div class='save-button-container'>";
  html +=
      "<button type='submit' aria-label='Save all station configurations'>Save "
      "Configuration</button>";
  html += "</div>";
  html += "</form>";
  return html;
}

String WiFiManager::generateStationTable() const {
  String html;
  auto& stationManager = StationManager::getInstance();

  WaveBand currentBand = WaveBand::LONG_WAVE;
  bool firstBand = true;

  // Helper function to get CSS class for each band
  auto getBandClass = [](WaveBand band) -> String {
    switch (band) {
      case WaveBand::LONG_WAVE:
        return "long-wave";
      case WaveBand::MEDIUM_WAVE:
        return "medium-wave";
      case WaveBand::SHORT_WAVE:
        return "short-wave";
      default:
        return "";
    }
  };

  // Initialize sections for each wave band
  String longWaveHtml =
      "<div class='wave-band long-wave' role='region' aria-label='Long Wave'><h2>Long Wave</h2>";
  String mediumWaveHtml =
      "<div class='wave-band medium-wave' role='region' aria-label='Medium Wave'><h2>Medium "
      "Wave</h2>";
  String shortWaveHtml =
      "<div class='wave-band short-wave' role='region' aria-label='Short Wave'><h2>Short Wave</h2>";

  // Track if each band has stations
  bool hasLongWave = false;
  bool hasMediumWave = false;
  bool hasShortWave = false;

  // First collect stations for each band
  for (size_t i = 0; i < stationManager.getStationCount(); i++) {
    const Station* station = stationManager.getStation(i);
    if (!station) continue;

    String stationHtml =
        "<div class='station' role='group' aria-label='Station " + String(i + 1) + "'>";
    stationHtml += "<div class='station-header'>";
    stationHtml += "<h3 class='station-name'>" + String(station->getName()) + "</h3>";
    stationHtml += "<div class='enable-toggle'>";
    stationHtml += "<input type='checkbox' id='enable_" + String(i) + "' name='enable_" +
                   String(i) + "' " + (station->isEnabled() ? "checked" : "") +
                   " aria-label='Enable station'>";
    stationHtml += "<label for='enable_" + String(i) + "'>Enabled</label>";
    stationHtml += "</div>";
    stationHtml += "</div>";

    stationHtml += "<div class='input-group' role='group' aria-label='Frequency control'>";
    stationHtml += "<input type='number' id='freq_" + String(i) + "' name='freq_" + String(i) +
                   "' value='" + String(station->getFrequency()) +
                   "' aria-label='Station frequency'>";
    stationHtml += "<button type='button' class='tune-button' onclick='setFrequency(\"freq_" +
                   String(i) + "\")' aria-label='Set current tuning value'>Set</button>";
    stationHtml += "</div>";

    stationHtml += "<input type='text' name='msg_" + String(i) + "' value='" +
                   String(station->getMessage()) +
                   "' placeholder='Message' aria-label='Station message'>";
    stationHtml += "</div>";

    // Add to the appropriate band
    switch (station->getBand()) {
      case WaveBand::LONG_WAVE:
        longWaveHtml += stationHtml;
        hasLongWave = true;
        break;
      case WaveBand::MEDIUM_WAVE:
        mediumWaveHtml += stationHtml;
        hasMediumWave = true;
        break;
      case WaveBand::SHORT_WAVE:
        shortWaveHtml += stationHtml;
        hasShortWave = true;
        break;
    }
  }

  // Add closing div tags
  longWaveHtml += "</div>";
  mediumWaveHtml += "</div>";
  shortWaveHtml += "</div>";

  // Add each section to the final HTML if it has stations
  if (hasLongWave) html += longWaveHtml;
  if (hasMediumWave) html += mediumWaveHtml;
  if (hasShortWave) html += shortWaveHtml;

  return html;
}

String WiFiManager::generateStatusJson() const {
  String json = "{";
  json += "\"wifiEnabled\":" + String(wifiEnabled ? "true" : "false") + ",";
  json += "\"uptime\":" + String((millis() - startTime) / 1000) + ",";
  json += "\"timeoutIn\":" + String((DEFAULT_TIMEOUT - (millis() - startTime)) / 1000);
  json += "}";
  return json;
}

void WiFiManager::handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: " + server.uri() + "\n";
  message += "Method: " + String(server.method() == HTTP_GET ? "GET" : "POST") + "\n";
  server.send(404, "text/plain", message);
}

void WiFiManager::handleStationConfig() {
  server.send(200, "text/html", generateHTML(generateStationTable()));
}

void WiFiManager::handleAPI() { server.send(200, "application/json", generateStatusJson()); }

void WiFiManager::updateStatusLED() {
  if (wifiEnabled) {
    unsigned long currentTime = millis();
    if (currentTime - lastLedFlash >= LED_FLASH_INTERVAL) {
      flashLED();
      lastLedFlash = currentTime;
    }
  }
}

void WiFiManager::flashLED() {
  static bool ledState = false;
  ledState = !ledState;
  digitalWrite(LED_BUILTIN, ledState);
}

String WiFiManager::generateHTML(const String& content) const {
  String html = String(HTML_HEADER);
  html += CSS_STYLES;
  html += "</style>";
  html += "<script>";
  html += JAVASCRIPT_CODE;
  html += "</script>";
  html += "</head><body>";
  html += content;
  html += HTML_FOOTER;
  return html;
}
