#include "WiFiManager.h"
#include <ArduinoJson.h>

// Define static members
const char* WiFiManager::HTML_HEADER = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Radio Configuration</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>)";

const char* WiFiManager::CSS_STYLES = R"(
    body { 
        font-family: Arial, sans-serif; 
        max-width: 800px; 
        margin: 0 auto; 
        padding: 20px;
        background-color: #f0f0f0;
        padding-bottom: 80px;
        padding-top: 70px;
    }
    .station { 
        background: #ffffff; 
        padding: 15px; 
        margin: 10px 0; 
        border-radius: 5px;
        box-shadow: 0 2px 4px rgba(0,0,0,0.1);
    }
    .station-header {
        display: flex;
        justify-content: space-between;
        align-items: center;
        margin-bottom: 15px;
    }
    .station-name {
        margin: 0;
        font-size: 1.17em;
    }
    .input-group {
        display: flex;
        margin-bottom: 10px;
    }
    .input-group input {
        border-top-right-radius: 0;
        border-bottom-right-radius: 0;
        border-right: none;
    }
    .input-group button {
        border-top-left-radius: 0;
        border-bottom-left-radius: 0;
        border: 1px solid #ddd;
        background: #e9ecef;
        cursor: pointer;
        color: #212529;
        font-weight: 500;
        min-width: 50px;
    }
    .input-group button:hover {
        background: #dee2e6;
    }
    .enable-toggle {
        display: flex;
        align-items: center;
        gap: 8px;
        white-space: nowrap;
    }
    input[type="checkbox"] {
        width: 18px;
        height: 18px;
        margin: 0;
    }
    input[type="number"] {
        width: 60px;
        padding: 8px;
        border: 1px solid #ddd;
        border-radius: 4px;
    }
    input[type="text"] {
        width: 100%;
        padding: 8px;
        border: 1px solid #ddd;
        border-radius: 4px;
    }
    .tune-button {
        padding: 8px;
        white-space: nowrap;
    }
    .wave-band { 
        padding: 10px; 
        margin: 15px 0; 
        border-radius: 3px;
    }
    .wave-band:nth-of-type(1) {
        background: #e3f2fd;
        border-left: 4px solid #2196F3;
    }
    .wave-band:nth-of-type(2) {
        background: #e8f5e9;
        border-left: 4px solid #4CAF50;
    }
    .wave-band:nth-of-type(3) {
        background: #f3e5f5;
        border-left: 4px solid #9c27b0;
    }
    label {
        display: block;
        width: 100%;
        margin-top: 15px;
    }
    button { 
        background: #4CAF50; 
        color: white; 
        padding: 12px 20px; 
        border: none; 
        border-radius: 5px; 
        cursor: pointer;
        transition: background-color 0.3s;
        font-size: 16px;
    }
    button:hover { 
        background: #45a049; 
    }
    .tune-button { 
        background: #2196F3;
        min-width: 120px;
        white-space: nowrap;
    }
    .tune-button:hover { 
        background: #1976D2; 
    }
    button[type="submit"] {
        background-color: #4CAF50;
        color: white;
        padding: 12px 20px;
        border: none;
        border-radius: 4px;
        cursor: pointer;
        width: 100%;
        font-size: 16px;
        margin-top: 15px;
    }
    button[type="submit"]:disabled {
        background-color: #cccccc;
        cursor: not-allowed;
        opacity: 0.7;
    }
    button[type="submit"].saving {
        position: relative;
        color: transparent;
    }
    button[type="submit"].saving::after {
        content: 'Saving...';
        position: absolute;
        left: 50%;
        top: 50%;
        transform: translate(-50%, -50%);
        color: white;
    }
    .status { 
        position: fixed;
        top: 0;
        left: 0;
        right: 0;
        background: rgba(255, 255, 255, 0.95);
        padding: 15px;
        box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        text-align: center;
        z-index: 1000;
        font-size: 1.1em;
        font-weight: bold;
        color: #2196F3;
    }
    .save-button-container {
        position: fixed;
        bottom: 0;
        left: 0;
        right: 0;
        background: rgba(255, 255, 255, 0.95);
        padding: 15px;
        box-shadow: 0 -2px 10px rgba(0,0,0,0.1);
        text-align: center;
        z-index: 1000;
    }
    .save-button-container button {
        width: 100%;
        max-width: 800px;
        margin: 0 auto;
        font-weight: bold;
    }
    .toast {
        position: fixed;
        top: 70px;
        left: 50%;
        transform: translateX(-50%);
        background: #4CAF50;
        color: white;
        padding: 12px 24px;
        border-radius: 4px;
        box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        z-index: 2000;
        font-weight: bold;
        opacity: 0;
        transition: opacity 0.3s ease-in-out;
    }
    .toast.error {
    }
    .toast.show {
        opacity: 1;
    }
    h1 {
        color: #333;
        border-bottom: 2px solid #2196F3;
        padding-bottom: 10px;
        margin: 0 0 20px 0;
        font-size: 24px;
    }
    h2 {
        color: #444;
        margin: 0 0 15px 0;
        font-size: 20px;
    }
    h3 {
        color: #666;
        margin: 0 0 10px 0;
        font-size: 18px;
    }
    @media (max-width: 600px) {
        body {
            padding: 15px;
            padding-bottom: 80px;
            padding-top: 70px;
        }
        .freq-group {
            flex-direction: column;
            gap: 8px;
        }
        .tune-button {
            width: 100%;
        }
        input[type="number"] {
            width: 100%;
        }
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

    // Update tuning value every 100ms
    setInterval(updateTuningValue, 100);

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

  // Only check timeout when running on battery power
  if (!PowerManager::getInstance().isUSBPowered() && millis() - startTime > DEFAULT_TIMEOUT) {
    stop();
    return;
  }

  updateStatusLED();
}

void WiFiManager::setupServer() {
  server.on("/", HTTP_GET, [this]() { handleRoot(); });
  server.on("/save", HTTP_POST, [this]() { handleSaveConfig(); });
  server.on("/tuning", HTTP_GET, [this]() { handleGetTuningValue(); });
  server.on("/stations", HTTP_GET, [this]() { handleStationConfig(); });
  server.on("/api/status", HTTP_GET, [this]() { handleAPI(); });
  server.onNotFound([this]() { handleNotFound(); });
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
  int tuningValue = analogRead(Pins::TUNING_POT);
  String response = "{\"value\":" + String(tuningValue) + "}";
  server.send(200, "application/json", response);
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
  DynamicJsonDocument doc(4096);  // Increased buffer size for larger JSON
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
  DynamicJsonDocument response(256);
  response["success"] = success;
  response["message"] = message;

  String responseStr;
  serializeJson(response, responseStr);
  server.send(200, "application/json", responseStr);
}

String WiFiManager::generateConfigPage() const {
  String html = "<div class='status'>Current Tuning Value: <span id='currentTuning'>-</span></div>";
  html += "<h1>Radio Configuration</h1>";
  html += "<form method='POST' action='/save'>";
  html += generateStationTable();
  html += "<div class='save-button-container'>";
  html += "<button type='submit'>Save Configuration</button>";
  html += "</div>";
  html += "</form>";
  return html;
}

String WiFiManager::generateStationTable() const {
  String html;
  auto& stationManager = StationManager::getInstance();

  WaveBand currentBand = WaveBand::LONG_WAVE;
  bool firstBand = true;

  for (size_t i = 0; i < stationManager.getStationCount(); i++) {
    const Station* station = stationManager.getStation(i);
    if (!station) continue;

    if (station->getBand() != currentBand) {
      if (!firstBand) html += "</div>";
      currentBand = station->getBand();
      html += "<div class='wave-band'><h2>" + String(toString(currentBand)) + "</h2>";
      firstBand = false;
    }

    html += "<div class='station'>";
    html += "<div class='station-header'>";
    html += "<h3 class='station-name'>" + String(station->getName()) + "</h3>";
    html += "<div class='enable-toggle'>";
    html += "<input type='checkbox' id='enable_" + String(i) + "' name='enable_" + String(i) +
            "' " + (station->isEnabled() ? "checked" : "") + ">";
    html += "<label for='enable_" + String(i) + "'>Enabled</label>";
    html += "</div>";
    html += "</div>";

    html += "<div class='input-group'>";
    html += "<input type='number' id='freq_" + String(i) + "' name='freq_" + String(i) +
            "' value='" + String(station->getFrequency()) + "'>";
    html += "<button type='button' class='tune-button' " + String("onclick='setFrequency(\"freq_") +
            String(i) + "\")'>Set</button>";
    html += "</div>";

    html += "<input type='text' name='msg_" + String(i) + "' value='" +
            String(station->getMessage()) + "' placeholder='Message'>";
    html += "</div>";
  }

  if (!firstBand) html += "</div>";
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
