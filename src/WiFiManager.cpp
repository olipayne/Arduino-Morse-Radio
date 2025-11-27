#include "WiFiManager.h"
#include <ArduinoJson.h>
#include <ElegantOTA.h>
#include "Version.h"  // Include the auto-generated version header
#include "WaveBandManager.h"

// Define static members - stored in PROGMEM to save RAM
const char WiFiManager::HTML_HEADER[] PROGMEM = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <title>Radio Configuration</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta name="theme-color" content="#2196F3">
    <style>)";

const char WiFiManager::CSS_STYLES[] PROGMEM = R"(
    :root {
        --primary-color: #2196F3;
        --secondary-color: #757575;
        --success-color: #4CAF50;
        --error-color: #f44336;
        --warning-color: #ff9800;
        --background-color: #f8f9fa;
        --card-background: #ffffff;
        --text-color: #212529;
        --text-muted: #6c757d;
        --border-color: #dee2e6;
        --primary-hover: #1976d2;
        --secondary-hover: #616161;
        --success-hover: #388e3c;
        --spacing: 1rem;
        --border-radius: 0.5rem;
        --shadow: 0 2px 4px rgba(0,0,0,0.1);
        --focus-shadow: 0 0 0 3px rgba(33, 150, 243, 0.1);
        --floating-shadow: 0 4px 12px rgba(0,0,0,0.15);
        --button-shadow: 0 2px 8px rgba(0,0,0,0.1);
        
        /* Wave band colors - matching LED colors */
        --long-wave-color: #dc3545;
        --medium-wave-color: #ffc107;
        --short-wave-color: #0d6efd;
        
        /* Header and help section backgrounds */
        --header-background: rgba(0,0,0,0.02);
        --help-background: #f8f9fa;
        --help-border: #e9ecef;
    }

    @media (prefers-color-scheme: dark) {
        :root {
            --background-color: #121212;
            --card-background: #1e1e1e;
            --text-color: #ffffff;
            --text-muted: #adb5bd;
            --border-color: #495057;
            --header-background: rgba(255,255,255,0.05);
            --help-background: #2d2d2d;
            --help-border: #404040;
            --primary-hover: #1565c0;
            --secondary-hover: #757575;
            --success-hover: #2e7d32;
            --shadow: 0 2px 4px rgba(255,255,255,0.05);
            --focus-shadow: 0 0 0 3px rgba(33, 150, 243, 0.2);
            --floating-shadow: 0 4px 12px rgba(0,0,0,0.3);
            --button-shadow: 0 2px 8px rgba(0,0,0,0.2);
        }
    }

    * {
        box-sizing: border-box;
        margin: 0;
        padding: 0;
    }

    body { 
        font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
        line-height: 1.6;
        background-color: var(--background-color);
        color: var(--text-color);
        min-height: 100vh;
    }

    .container {
        max-width: 600px;
        margin: 0 auto;
        padding: var(--spacing);
        padding-bottom: calc(var(--spacing) * 6);
        min-height: 100vh;
        display: flex;
        flex-direction: column;
    }

    .header {
        text-align: center;
        padding: calc(var(--spacing) * 2) 0;
        border-bottom: 1px solid var(--border-color);
        margin-bottom: var(--spacing);
    }

    .status-bar {
        background: var(--card-background);
        padding: calc(var(--spacing) * 0.75);
        border-radius: var(--border-radius);
        box-shadow: var(--shadow);
        margin-bottom: var(--spacing);
        text-align: center;
        font-size: 0.9em;
        color: var(--text-muted);
    }

    .nav {
        display: flex;
        gap: calc(var(--spacing) * 0.5);
        margin-bottom: calc(var(--spacing) * 1.5);
    }

    .nav a {
        flex: 1;
        padding: calc(var(--spacing) * 0.75);
        text-decoration: none;
        color: var(--text-color);
        background: var(--card-background);
        border-radius: var(--border-radius);
        text-align: center;
        font-weight: 500;
        border: 1px solid var(--border-color);
        transition: all 0.2s ease;
    }

    .nav a.active {
        background: var(--primary-color);
        color: white;
        border-color: var(--primary-color);
    }

    .nav a:hover:not(.active) {
        background: var(--border-color);
    }

    .card {
        background: var(--card-background);
        border: 1px solid var(--border-color);
        border-radius: var(--border-radius);
        box-shadow: var(--shadow);
        margin-bottom: var(--spacing);
        overflow: hidden;
    }

    .card-header {
        padding: var(--spacing);
        border-bottom: 1px solid var(--border-color);
        background: var(--header-background);
    }

    .card-body {
        padding: var(--spacing);
    }

    .station {
        border: 1px solid var(--border-color);
        border-radius: var(--border-radius);
        margin-bottom: calc(var(--spacing) * 0.75);
        overflow: hidden;
    }

    .station-header {
        display: flex;
        justify-content: space-between;
        align-items: center;
        padding: calc(var(--spacing) * 0.75);
        background: var(--header-background);
        border-bottom: 1px solid var(--border-color);
    }

    .station-name {
        font-weight: 600;
        color: var(--text-color);
    }

    .station-body {
        padding: var(--spacing);
    }

    .form-group {
        margin-bottom: var(--spacing);
    }

    .form-row {
        display: flex;
        gap: calc(var(--spacing) * 0.5);
        align-items: center;
    }

    label {
        display: block;
        margin-bottom: calc(var(--spacing) * 0.25);
        font-weight: 500;
        color: var(--text-color);
        font-size: 0.9em;
    }

    input, button {
        font-size: 16px;
        border-radius: var(--border-radius);
        padding: calc(var(--spacing) * 0.75);
        border: 1px solid var(--border-color);
        background: var(--card-background);
        color: var(--text-color);
        transition: border-color 0.2s ease, box-shadow 0.2s ease;
    }

    input:focus {
        outline: none;
        border-color: var(--primary-color);
        box-shadow: var(--focus-shadow);
    }

    input[type="text"], input[type="number"] {
        width: 100%;
    }

    input[type="number"] {
        -moz-appearance: textfield;
    }

    input[type="number"]::-webkit-outer-spin-button,
    input[type="number"]::-webkit-inner-spin-button {
        -webkit-appearance: none;
    }

    input[type="checkbox"] {
        width: 20px;
        height: 20px;
        margin: 0;
        accent-color: var(--primary-color);
    }

    .toggle {
        display: flex;
        align-items: center;
        gap: calc(var(--spacing) * 0.5);
    }

    button {
        background: var(--primary-color);
        color: white;
        border: none;
        font-weight: 500;
        cursor: pointer;
        transition: all 0.2s ease;
        min-height: 44px;
        display: inline-flex;
        align-items: center;
        justify-content: center;
    }

    button:hover {
        background: var(--primary-hover);
        transform: translateY(-1px);
    }

    button:active {
        transform: translateY(0);
    }

    .btn-secondary {
        background: var(--secondary-color);
    }

    .btn-secondary:hover {
        background: var(--secondary-hover);
    }

    .btn-success {
        background: var(--success-color);
    }

    .btn-success:hover {
        background: var(--success-hover);
    }

    .wave-band {
        margin-bottom: calc(var(--spacing) * 1.5);
    }

    .wave-band h2 {
        font-size: 1.1em;
        font-weight: 600;
        margin-bottom: var(--spacing);
        padding: calc(var(--spacing) * 0.5);
        border-left: 4px solid;
        background: var(--header-background);
    }

    .wave-band.long-wave h2 {
        border-left-color: var(--long-wave-color);
        color: var(--long-wave-color);
    }

    .wave-band.medium-wave h2 {
        border-left-color: var(--medium-wave-color);
        color: var(--medium-wave-color);
    }

    .wave-band.short-wave h2 {
        border-left-color: var(--short-wave-color);
        color: var(--short-wave-color);
    }

    .save-area {
        margin-top: auto;
        padding-top: calc(var(--spacing) * 2);
        border-top: 1px solid var(--border-color);
    }

    .save-button {
        width: 100%;
        padding: calc(var(--spacing) * 1);
        font-size: 1.1em;
        font-weight: 600;
    }

    .save-button:disabled {
        opacity: 0.5;
        cursor: not-allowed;
    }

    .save-button.saving {
        position: relative;
        color: transparent;
    }

    .save-button.saving::after {
        content: 'Saving...';
        position: absolute;
        left: 50%;
        top: 50%;
        transform: translate(-50%, -50%);
        color: white;
    }

    .calibration-display {
        text-align: center;
        padding: calc(var(--spacing) * 2);
        background: var(--card-background);
        border: 1px solid var(--border-color);
        border-radius: var(--border-radius);
        margin-bottom: calc(var(--spacing) * 1.5);
    }

    .tuning-value {
        font-size: 2em;
        font-weight: 700;
        color: var(--primary-color);
        margin-bottom: calc(var(--spacing) * 0.5);
    }

    .calibration-help {
        background: var(--help-background);
        border: 1px solid var(--help-border);
        border-radius: var(--border-radius);
        padding: var(--spacing);
        margin-bottom: calc(var(--spacing) * 1.5);
        font-size: 0.9em;
        line-height: 1.5;
        color: var(--text-color);
    }

    .frequency-display {
        font-weight: 600;
        color: var(--primary-color);
        font-size: 1.1em;
    }

    .floating-save {
        position: fixed;
        bottom: calc(var(--spacing) * 2);
        left: 50%;
        transform: translateX(-50%);
        z-index: 1000;
        box-shadow: var(--floating-shadow);
        border-radius: var(--border-radius);
        backdrop-filter: blur(8px);
    }

    .floating-save .save-button {
        padding: calc(var(--spacing) * 1);
        font-size: 1.1em;
        font-weight: 600;
        min-width: 200px;
        border-radius: var(--border-radius);
        box-shadow: var(--button-shadow);
    }

    .import-export-section {
        margin-top: calc(var(--spacing) * 2);
        padding: var(--spacing);
        padding-bottom: calc(var(--spacing) * 5);
        border-top: 1px solid var(--border-color);
        margin-bottom: calc(var(--spacing) * 2);
    }

    .import-export-buttons {
        display: flex;
        gap: calc(var(--spacing) * 0.75);
        margin-top: calc(var(--spacing) * 0.75);
    }

    .import-export-buttons button {
        flex: 1;
        font-size: 0.95em;
    }

    .btn-export {
        background: var(--secondary-color);
    }

    .btn-export:hover {
        background: var(--secondary-hover);
    }

    .btn-import {
        background: var(--primary-color);
    }

    .btn-import:hover {
        background: var(--primary-hover);
    }

    #importFile {
        display: none;
    }

    .toast {
        position: fixed;
        top: calc(var(--spacing) * 2);
        left: 50%;
        transform: translateX(-50%);
        background: var(--success-color);
        color: white;
        padding: calc(var(--spacing) * 0.75) var(--spacing);
        border-radius: var(--border-radius);
        box-shadow: var(--shadow);
        z-index: 1000;
        font-weight: 500;
        opacity: 0;
        transition: opacity 0.3s ease;
        pointer-events: none;
    }

    .toast.error {
        background: var(--error-color);
    }

    .toast.show {
        opacity: 1;
    }

    h1 {
        font-size: 1.5em;
        font-weight: 700;
        margin: 0;
        color: var(--text-color);
    }

    h2 {
        font-size: 1.2em;
        font-weight: 600;
        margin: 0 0 var(--spacing) 0;
        color: var(--text-color);
    }

    .text-muted {
        color: var(--text-muted);
        font-size: 0.9em;
    }

    /* Mobile-first responsive design */
    @media (max-width: 480px) {
        .container {
            padding: calc(var(--spacing) * 0.75);
        }
        
        .form-row {
            flex-direction: column;
            align-items: stretch;
        }
        
        .nav {
            flex-direction: column;
        }
        
        .nav a {
            padding: calc(var(--spacing) * 1);
        }
    }

    @media (hover: none) {
        button:hover {
            transform: none;
        }
    }

    /* Focus styles for accessibility */
    :focus-visible {
        outline: 2px solid var(--primary-color);
        outline-offset: 2px;
    }

    /* Loading animation */
    @keyframes pulse {
        0%, 100% { opacity: 1; }
        50% { opacity: 0.5; }
    }

    .saving {
        animation: pulse 1.5s infinite;
    }

    /* Battery indicator styles */
    .battery-indicator {
        display: flex;
        align-items: center;
        gap: calc(var(--spacing) * 0.75);
        padding: calc(var(--spacing) * 1);
        background: var(--header-background);
        border-radius: var(--border-radius);
        margin-bottom: var(--spacing);
    }

    .battery-icon {
        position: relative;
        width: 48px;
        height: 24px;
        border: 2px solid var(--text-color);
        border-radius: 3px;
        display: flex;
        align-items: center;
        padding: 2px;
    }

    .battery-icon::after {
        content: '';
        position: absolute;
        right: -4px;
        top: 50%;
        transform: translateY(-50%);
        width: 3px;
        height: 12px;
        background: var(--text-color);
        border-radius: 0 2px 2px 0;
    }

    .battery-fill {
        height: 100%;
        background: var(--success-color);
        border-radius: 2px;
        transition: width 0.5s ease, background-color 0.3s ease;
    }

    .battery-fill.low {
        background: var(--warning-color);
    }

    .battery-fill.critical {
        background: var(--error-color);
    }

    .battery-info {
        flex: 1;
        display: flex;
        flex-direction: column;
        gap: calc(var(--spacing) * 0.25);
    }

    .battery-percentage {
        font-size: 1.2em;
        font-weight: 600;
        color: var(--text-color);
    }

    .battery-voltage {
        font-size: 0.9em;
        color: var(--text-muted);
    }

    .battery-status {
        display: flex;
        align-items: center;
        gap: calc(var(--spacing) * 0.25);
        font-size: 0.85em;
        color: var(--primary-color);
        font-weight: 500;
    }

    .charging-icon {
        display: inline-block;
        animation: pulse 2s infinite;
    }

    @keyframes charging-pulse {
        0%, 100% { opacity: 1; }
        50% { opacity: 0.6; }
    }

    .battery-fill.charging {
        background: var(--primary-color);
        animation: charging-pulse 2s infinite;
    }
)";

const char WiFiManager::HTML_FOOTER[] PROGMEM = R"(
    </body>
</html>)";

const char WiFiManager::JAVASCRIPT_CODE[] PROGMEM = R"(
    function updateTuningValue() {
        const element = document.getElementById('currentTuning');
        if (!element) return;
        
        fetch('/tuning')
            .then(response => response.json())
            .then(data => {
                element.textContent = data.value;
            })
            .catch(error => console.error('Error fetching tuning value:', error));
    }

    function setFrequency(inputId) {
        fetch('/tuning')
            .then(response => response.json())
            .then(data => {
                const input = document.getElementById(inputId);
                if (input) {
                    input.value = data.value;
                    input.style.backgroundColor = 'var(--success-color)';
                    input.style.opacity = '0.3';
                    setTimeout(() => {
                        input.style.backgroundColor = '';
                        input.style.opacity = '';
                    }, 500);
                    
                    // Save the frequency immediately
                    const stationIndex = inputId.replace('freq_', '');
                    saveFrequency(stationIndex, data.value);
                }
            })
            .catch(error => console.error('Error setting frequency:', error));
    }

    function saveFrequency(stationIndex, frequency) {
        const data = {
            station: parseInt(stationIndex),
            frequency: parseInt(frequency)
        };

        fetch('/save-frequency', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(data)
        })
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                showToast('Frequency saved: ' + frequency);
            } else {
                showToast('Failed to save frequency', true);
            }
        })
        .catch(error => {
            console.error('Error:', error);
            showToast('Error saving frequency', true);
        });
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
        // Look for submit button inside form first, then outside (for floating button)
        let submitButton = form.querySelector('button[type="submit"]');
        if (!submitButton) {
            submitButton = document.querySelector('button[form="' + form.id + '"]');
        }
        
        // Disable button and show saving state
        if (submitButton) {
            submitButton.disabled = true;
            submitButton.classList.add('saving');
        }
        
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
            if (submitButton) {
                submitButton.disabled = false;
                submitButton.classList.remove('saving');
            }
        });
    }

    function exportMessages() {
        fetch('/api/messages')
            .then(response => response.json())
            .then(data => {
                // Create a blob from the JSON data
                const blob = new Blob([JSON.stringify(data, null, 2)], { type: 'application/json' });
                const url = URL.createObjectURL(blob);
                
                // Create a temporary link and trigger download
                const a = document.createElement('a');
                a.href = url;
                a.download = 'radio-messages-' + new Date().toISOString().split('T')[0] + '.json';
                document.body.appendChild(a);
                a.click();
                
                // Clean up
                document.body.removeChild(a);
                URL.revokeObjectURL(url);
                
                showToast('Messages exported successfully');
            })
            .catch(error => {
                console.error('Error exporting messages:', error);
                showToast('Error exporting messages', true);
            });
    }

    function importMessages() {
        const input = document.getElementById('importFile');
        if (!input || !input.files || input.files.length === 0) {
            showToast('Please select a file to import', true);
            return;
        }

        const file = input.files[0];
        const reader = new FileReader();

        reader.onload = function(e) {
            try {
                const jsonData = JSON.parse(e.target.result);
                
                fetch('/api/messages', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                    },
                    body: JSON.stringify(jsonData)
                })
                .then(response => response.json())
                .then(data => {
                    if (data.success) {
                        showToast(data.message);
                        // Reload the page to show updated messages
                        setTimeout(() => window.location.reload(), 1500);
                    } else {
                        showToast(data.message, true);
                    }
                })
                .catch(error => {
                    console.error('Error importing messages:', error);
                    showToast('Error importing messages', true);
                });
            } catch (error) {
                console.error('Error parsing JSON:', error);
                showToast('Invalid JSON file', true);
            }
        };

        reader.onerror = function() {
            showToast('Error reading file', true);
        };

        reader.readAsText(file);
    }

    function triggerFileInput() {
        const input = document.getElementById('importFile');
        if (input) {
            input.click();
        }
    }

    // Update tuning value every 500ms
    setInterval(updateTuningValue, 500);

    // Add form submit handler
    document.addEventListener('DOMContentLoaded', () => {
        const form = document.querySelector('form');
        if (form) {
            form.addEventListener('submit', handleFormSubmit);
        }
        
        // Add import file input change handler
        const importInput = document.getElementById('importFile');
        if (importInput) {
            importInput.addEventListener('change', importMessages);
        }
    });
)";

void WiFiManager::begin() {
  pinMode(Pins::WIFI_BUTTON, INPUT_PULLUP);
  pinMode(Pins::SW_LED, OUTPUT);
  digitalWrite(Pins::SW_LED, LOW);

  // Initialize timer for system tasks
  esp_timer_create_args_t timer_config = {.callback = nullptr,
                                          .arg = nullptr,
                                          .dispatch_method = ESP_TIMER_TASK,
                                          .name = "system_timer",
                                          .skip_unhandled_events = true};
  esp_timer_create(&timer_config, &timer);
}

void WiFiManager::toggle() {
  PowerManager::getInstance().resetActivityTimer(wifiEnabled ? "WiFi Disabled" : "WiFi Enabled");
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
    digitalWrite(Pins::SW_LED, LOW);
    
    // Restore the wave band LED state
    WaveBandManager::getInstance().updateLEDs();
    
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println(F("WiFi stopped"));
#endif
  }
}

void WiFiManager::startAP() {
  // Stop morse radio functionality when WiFi starts
  AudioManager::getInstance().stop();
  MorseCode::getInstance().stop();
  
  // Turn off all wave band LEDs (we'll repurpose SW_LED for WiFi status)
  WaveBandManager::getInstance().turnOffAllBandLEDs();
  
  WiFi.mode(WIFI_AP);

  const char* ssid = "MorseRadio";

  if (WiFi.softAP(ssid, nullptr, AP_CHANNEL, false, MAX_CONNECTIONS)) {
    setupServer();
    setupMDNS();

    wifiEnabled = true;
    startTime = millis();
    lastLedFlash = millis();

#ifdef DEBUG_SERIAL_OUTPUT
    Serial.print(F("WiFi AP started: "));
    Serial.println(ssid);
    Serial.print(F("AP IP address: "));
    Serial.println(WiFi.softAPIP());
    Serial.print(F("Free heap: "));
    Serial.println(ESP.getFreeHeap());
    Serial.print(F("Flash size: "));
    Serial.println(ESP.getFlashChipSize());
    Serial.print(F("Sketch size: "));
    Serial.println(ESP.getSketchSize());
    Serial.print(F("Free sketch space: "));
    Serial.println(ESP.getFreeSketchSpace());
#endif
  } else {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println(F("Failed to start WiFi AP"));
#endif
  }
}

void WiFiManager::handle() {
  if (!wifiEnabled) return;

  server.handleClient();
  ElegantOTA.loop();

  // WiFi does not auto-timeout - user must manually toggle it off
  // (The device itself has a separate inactivity timeout for deep sleep)

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
  server.on("/stations", HTTP_GET, [this]() { handleStationConfig(); });
  server.on("/calibration", HTTP_GET, [this]() { handleCalibration(); });
  server.on("/settings", HTTP_GET, [this]() { handleSettings(); });
  server.on("/save", HTTP_POST, [this]() { handleSaveConfig(); });
  server.on("/save-frequency", HTTP_POST, [this]() { handleSaveFrequency(); });
  server.on("/save-settings", HTTP_POST, [this]() { handleSaveSettings(); });
  server.on("/tuning", HTTP_GET, [this]() { handleGetTuningValue(); });
  server.on("/api/status", HTTP_GET, [this]() { handleAPI(); });
  server.on("/api/battery", HTTP_GET, [this]() { handleBatteryStatus(); });
  server.on("/api/messages", HTTP_GET, [this]() { handleExportMessages(); });
  server.on("/api/messages", HTTP_POST, [this]() { handleImportMessages(); });
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
  PowerManager::getInstance().resetActivityTimer("Web Interface - Home Page Viewed");
  server.send(200, "text/html", generateHTML(generateHomePage())); 
}

void WiFiManager::handleStationConfig() {
  PowerManager::getInstance().resetActivityTimer("Web Interface - Station Config Viewed");
  server.send(200, "text/html", generateHTML(generateStationPage()));
}

void WiFiManager::handleCalibration() {
  PowerManager::getInstance().resetActivityTimer("Web Interface - Calibration Viewed");
  server.send(200, "text/html", generateHTML(generateCalibrationPage()));
}

void WiFiManager::handleSettings() {
  PowerManager::getInstance().resetActivityTimer("Web Interface - Settings Viewed");
  server.send(200, "text/html", generateHTML(generateSettingsPage()));
}

void WiFiManager::handleGetTuningValue() {
  int tuningValue = PowerManager::getInstance().readADCRaw(Pins::TUNING_POT);
  String response = "{\"value\":" + String(tuningValue) + "}";
  server.send(200, "application/json", response);
  startTime = millis();  // Reset the timeout counter
  PowerManager::getInstance().resetActivityTimer("Web Interface - Tuning Value Request");
}

void WiFiManager::handleSaveFrequency() {
  String jsonData = server.arg("plain");

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.println("Received frequency data: " + jsonData);
#endif

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, jsonData);

  if (error) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("Failed to parse frequency JSON");
#endif
    server.send(400, "application/json", "{\"success\":false,\"message\":\"Invalid JSON\"}");
    return;
  }

  int stationIndex = doc["station"];
  int frequency = doc["frequency"];

  auto& stationManager = StationManager::getInstance();
  Station* station = stationManager.getStation(stationIndex);

  if (station) {
    station->setFrequency(frequency);
    stationManager.saveToPreferences();

#ifdef DEBUG_SERIAL_OUTPUT
    Serial.printf("Updated station %d frequency to %d\n", stationIndex, frequency);
#endif

    PowerManager::getInstance().resetActivityTimer("Web Interface - Frequency Saved");
    server.send(200, "application/json", "{\"success\":true,\"message\":\"Frequency saved\"}");
  } else {
    server.send(400, "application/json", "{\"success\":false,\"message\":\"Invalid station\"}");
  }
}

void WiFiManager::handleSaveSettings() {
  String jsonData = server.arg("plain");

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.println("Received settings data: " + jsonData);
#endif

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, jsonData);

  if (error) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("Failed to parse settings JSON");
#endif
    server.send(400, "application/json", "{\"success\":false,\"message\":\"Invalid JSON\"}");
    return;
  }

  unsigned int timeout = doc["inactivityTimeout"];
  
  auto& configManager = ConfigManager::getInstance();
  configManager.setInactivityTimeout(timeout);
  configManager.save();

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.printf("Updated device inactivity timeout to %d minutes\n", timeout);
#endif

  PowerManager::getInstance().resetActivityTimer("Web Interface - Settings Saved");
  server.send(200, "application/json", "{\"success\":true,\"message\":\"Settings saved\"}");
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
    PowerManager::getInstance().resetActivityTimer("Web Interface - Config Saved");
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

String WiFiManager::generateHomePage() const {
  String html;
  html.reserve(1024);

  html += F("<div class='container'>");
  html += F("<div class='header'>");
  html += F("<h1>Radio Configuration</h1>");
  html += F("<div class='text-muted'>Firmware: ");
  html += String(FIRMWARE_VERSION);
  html += F("</div>");
  html += F("</div>");

  html += F("<div class='status-bar'>");
  html += F("Current Tuning: <span id='currentTuning'>-</span>");
  html += F("</div>");

  html += F("<div class='nav'>");
  html += F("<a href='/stations' class='nav-link'>Messages</a>");
  html += F("<a href='/calibration' class='nav-link'>Tuning</a>");
  html += F("<a href='/settings' class='nav-link'>Settings</a>");
  html += F("</div>");

  html += F("<div class='card'>");
  html += F("<div class='card-header'>");
  html += F("<h2>Welcome</h2>");
  html += F("</div>");
  html += F("<div class='card-body'>");
  html += F("<p>Configure your radio station messages and tune each station frequency.</p>");
  html += F("<p><strong>Messages:</strong> Set custom morse code messages for each station</p>");
  html += F(
      "<p><strong>Tuning:</strong> Set the potentiometer position for each station frequency</p>");
  html += F("</div>");
  html += F("</div>");

  html += F("</div>");
  return html;
}

String WiFiManager::generateStationPage() const {
  String html;
  html.reserve(4096);

  html += F("<div class='container'>");
  html += F("<div class='header'>");
  html += F("<h1>Station Configuration</h1>");
  html += F("</div>");

  html += F("<div class='nav'>");
  html += F("<a href='/' class='nav-link'>Home</a>");
  html += F("<a href='/stations' class='nav-link active'>Messages</a>");
  html += F("<a href='/calibration' class='nav-link'>Tuning</a>");
  html += F("<a href='/settings' class='nav-link'>Settings</a>");
  html += F("</div>");

  html += F("<form method='POST' action='/save' id='stationForm'>");
  html += generateStationList();
  
  html += F("<div class='import-export-section'>");
  html += F("<h2>Import / Export</h2>");
  html += F("<p class='text-muted'>Export messages to backup or share with other devices. Import to restore or sync messages.</p>");
  html += F("<div class='import-export-buttons'>");
  html += F("<button type='button' onclick='exportMessages()' class='btn-export'>📥 Export</button>");
  html += F("<button type='button' onclick='triggerFileInput()' class='btn-import'>📤 Import</button>");
  html += F("</div>");
  html += F("</div>");
  
  html += F("</form>");
  
  html += F("<input type='file' id='importFile' accept='application/json,.json'>");

  html += F("<div class='floating-save'>");
  html +=
      F("<button type='submit' form='stationForm' class='save-button btn-success'>💾 Save "
        "Messages</button>");
  html += F("</div>");
  html += F("</div>");

  return html;
}

String WiFiManager::generateStationList() const {
  String html;
  auto& stationManager = StationManager::getInstance();

  // Initialize sections for each wave band
  String longWaveHtml = F("<div class='wave-band long-wave'><h2>Long Wave</h2>");
  String mediumWaveHtml = F("<div class='wave-band medium-wave'><h2>Medium Wave</h2>");
  String shortWaveHtml = F("<div class='wave-band short-wave'><h2>Short Wave</h2>");

  // Track if each band has stations
  bool hasLongWave = false;
  bool hasMediumWave = false;
  bool hasShortWave = false;

  // Generate stations for each band
  for (size_t i = 0; i < stationManager.getStationCount(); i++) {
    const Station* station = stationManager.getStation(i);
    if (!station) continue;

    String stationHtml = F("<div class='station'>");
    stationHtml += F("<div class='station-header'>");
    stationHtml += F("<div class='station-name'>");
    stationHtml += String(station->getName());
    stationHtml += F("</div>");
    stationHtml += F("<div class='toggle'>");
    stationHtml += F("<input type='checkbox' id='enable_");
    stationHtml += String(i);
    stationHtml += F("' name='enable_");
    stationHtml += String(i);
    stationHtml += F("' ");
    stationHtml += station->isEnabled() ? F("checked") : F("");
    stationHtml += F(">");
    stationHtml += F("<label for='enable_");
    stationHtml += String(i);
    stationHtml += F("'>Enabled</label>");
    stationHtml += F("</div>");
    stationHtml += F("</div>");

    stationHtml += F("<div class='station-body'>");
    stationHtml += F("<div class='form-group'>");
    stationHtml += F("<label>Message</label>");
    stationHtml += F("<input type='text' name='msg_");
    stationHtml += String(i);
    stationHtml += F("' value='");
    stationHtml += String(station->getMessage());
    stationHtml += F("' placeholder='Enter your morse code message'>");
    stationHtml += F("</div>");
    stationHtml += F("<!-- Hidden frequency input to maintain save functionality -->");
    stationHtml += F("<input type='hidden' name='freq_");
    stationHtml += String(i);
    stationHtml += F("' value='");
    stationHtml += String(station->getFrequency());
    stationHtml += F("'>");
    stationHtml += F("</div>");
    stationHtml += F("</div>");

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

  // Close wave band divs
  longWaveHtml += F("</div>");
  mediumWaveHtml += F("</div>");
  shortWaveHtml += F("</div>");

  // Add each section to the final HTML if it has stations
  if (hasLongWave) html += longWaveHtml;
  if (hasMediumWave) html += mediumWaveHtml;
  if (hasShortWave) html += shortWaveHtml;

  return html;
}

String WiFiManager::generateCalibrationPage() const {
  String html;
  html.reserve(4096);

  html += F("<div class='container'>");
  html += F("<div class='header'>");
  html += F("<h1>Station Tuning</h1>");
  html += F("</div>");

  html += F("<div class='nav'>");
  html += F("<a href='/' class='nav-link'>Home</a>");
  html += F("<a href='/stations' class='nav-link'>Messages</a>");
  html += F("<a href='/calibration' class='nav-link active'>Tuning</a>");
  html += F("<a href='/settings' class='nav-link'>Settings</a>");
  html += F("</div>");

  html += F("<div class='calibration-help'>");
  html += F("<strong>Tuning Instructions:</strong><br>");
  html +=
      F("Turn the tuning knob to the desired position for each station, then click 'Set' to save "
        "that frequency. ");
  html += F("The current potentiometer reading is shown in real-time.");
  html += F("</div>");

  html += F("<div class='calibration-display'>");
  html += F("<div class='tuning-value' id='currentTuning'>-</div>");
  html += F("<div class='text-muted'>Current Potentiometer Reading</div>");
  html += F("</div>");

  // Generate station list for calibration
  auto& stationManager = StationManager::getInstance();

  // Initialize sections for each wave band
  String longWaveHtml = F("<div class='wave-band long-wave'><h2>Long Wave</h2>");
  String mediumWaveHtml = F("<div class='wave-band medium-wave'><h2>Medium Wave</h2>");
  String shortWaveHtml = F("<div class='wave-band short-wave'><h2>Short Wave</h2>");

  bool hasLongWave = false;
  bool hasMediumWave = false;
  bool hasShortWave = false;

  for (size_t i = 0; i < stationManager.getStationCount(); i++) {
    const Station* station = stationManager.getStation(i);
    if (!station || !station->isEnabled()) continue;

    String stationHtml = F("<div class='station'>");
    stationHtml += F("<div class='station-header'>");
    stationHtml += F("<div class='station-name'>");
    stationHtml += String(station->getName());
    stationHtml += F("</div>");
    stationHtml += F("<div class='frequency-display'>");
    stationHtml += String(station->getFrequency());
    stationHtml += F("</div>");
    stationHtml += F("</div>");

    stationHtml += F("<div class='station-body'>");
    stationHtml += F("<div class='form-group'>");
    stationHtml += F("<div class='form-row'>");
    stationHtml += F("<input type='number' id='freq_");
    stationHtml += String(i);
    stationHtml += F("' value='");
    stationHtml += String(station->getFrequency());
    stationHtml += F("' readonly>");
    stationHtml += F("<button type='button' class='btn-primary' onclick='setFrequency(\"freq_");
    stationHtml += String(i);
    stationHtml += F("\")'>Set Current</button>");
    stationHtml += F("</div>");
    stationHtml += F("</div>");
    stationHtml += F("</div>");
    stationHtml += F("</div>");

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

  // Close wave band divs
  longWaveHtml += F("</div>");
  mediumWaveHtml += F("</div>");
  shortWaveHtml += F("</div>");

  // Add each section to the final HTML if it has stations
  if (hasLongWave) html += longWaveHtml;
  if (hasMediumWave) html += mediumWaveHtml;
  if (hasShortWave) html += shortWaveHtml;

  html += F("</div>");
  return html;
}

String WiFiManager::generateSettingsPage() const {
  String html;
  html.reserve(2048);

  html += F("<div class='container'>");
  html += F("<div class='header'>");
  html += F("<h1>Settings</h1>");
  html += F("</div>");

  html += F("<div class='nav'>");
  html += F("<a href='/' class='nav-link'>Home</a>");
  html += F("<a href='/stations' class='nav-link'>Messages</a>");
  html += F("<a href='/calibration' class='nav-link'>Tuning</a>");
  html += F("<a href='/settings' class='nav-link active'>Settings</a>");
  html += F("</div>");

  html += F("<div class='card'>");
  html += F("<div class='card-header'>");
  html += F("<h2>Battery Status</h2>");
  html += F("</div>");
  html += F("<div class='card-body'>");
  html += F("<div class='battery-indicator'>");
  html += F("<div class='battery-icon'>");
  html += F("<div class='battery-fill' id='batteryFill' style='width: 0%'></div>");
  html += F("</div>");
  html += F("<div class='battery-info'>");
  html += F("<div class='battery-percentage' id='batteryPercentage'>--%</div>");
  html += F("<div class='battery-voltage' id='batteryVoltage'>-- V</div>");
  html += F("<div class='battery-status' id='batteryStatus' style='display: none;'>");
  html += F("<span class='charging-icon'>⚡</span>");
  html += F("<span>Charging</span>");
  html += F("</div>");
  html += F("</div>");
  html += F("</div>");
  html += F("</div>");
  html += F("</div>");

  html += F("<div class='card'>");
  html += F("<div class='card-header'>");
  html += F("<h2>Power Settings</h2>");
  html += F("</div>");
  html += F("<div class='card-body'>");
  html += F("<form id='settingsForm' onsubmit='saveSettings(event)'>");
  
  html += F("<div class='form-group'>");
  html += F("<label for='inactivityTimeout'>Inactivity Timeout (minutes)</label>");
  html += F("<input type='number' id='inactivityTimeout' name='inactivityTimeout' min='5' max='360' value='");
  html += String(ConfigManager::getInstance().getInactivityTimeout());
  html += F("' required>");
  html += F("<small class='text-muted'>Device will automatically enter deep sleep after this many minutes without any input changes. Range: 5-360 minutes.</small>");
  html += F("</div>");

  html += F("<button type='submit' class='save-button btn-success'>💾 Save Settings</button>");
  html += F("</form>");
  html += F("</div>");
  html += F("</div>");

  html += F("<script>");
  html += F("function updateBatteryStatus() {");
  html += F("  fetch('/api/battery')");
  html += F("    .then(response => response.json())");
  html += F("    .then(data => {");
  html += F("      const fillElement = document.getElementById('batteryFill');");
  html += F("      const percentageElement = document.getElementById('batteryPercentage');");
  html += F("      const voltageElement = document.getElementById('batteryVoltage');");
  html += F("      const statusElement = document.getElementById('batteryStatus');");
  html += F("      ");
  html += F("      if (fillElement && percentageElement && voltageElement && statusElement) {");
  html += F("        percentageElement.textContent = data.percentage + '%';");
  html += F("        voltageElement.textContent = data.voltage.toFixed(2) + ' V';");
  html += F("        fillElement.style.width = data.percentage + '%';");
  html += F("        ");
  html += F("        fillElement.classList.remove('low', 'critical', 'charging');");
  html += F("        if (data.isCharging) {");
  html += F("          fillElement.classList.add('charging');");
  html += F("          statusElement.style.display = 'flex';");
  html += F("        } else {");
  html += F("          statusElement.style.display = 'none';");
  html += F("          if (data.percentage < 10) {");
  html += F("            fillElement.classList.add('critical');");
  html += F("          } else if (data.percentage < 30) {");
  html += F("            fillElement.classList.add('low');");
  html += F("          }");
  html += F("        }");
  html += F("      }");
  html += F("    })");
  html += F("    .catch(error => console.error('Error fetching battery status:', error));");
  html += F("}");
  html += F("");
  html += F("function saveSettings(event) {");
  html += F("  event.preventDefault();");
  html += F("  const form = event.target;");
  html += F("  const submitButton = form.querySelector('button[type=\"submit\"]');");
  html += F("  submitButton.disabled = true;");
  html += F("  submitButton.classList.add('saving');");
  html += F("  const timeout = parseInt(document.getElementById('inactivityTimeout').value);");
  html += F("  if (timeout < 5 || timeout > 360) {");
  html += F("    showToast('Timeout must be between 5 and 360 minutes', true);");
  html += F("    submitButton.disabled = false;");
  html += F("    submitButton.classList.remove('saving');");
  html += F("    return;");
  html += F("  }");
  html += F("  fetch('/save-settings', {");
  html += F("    method: 'POST',");
  html += F("    headers: { 'Content-Type': 'application/json' },");
  html += F("    body: JSON.stringify({ inactivityTimeout: timeout })");
  html += F("  })");
  html += F("  .then(response => response.json())");
  html += F("  .then(data => {");
  html += F("    showToast(data.message, !data.success);");
  html += F("  })");
  html += F("  .catch(error => {");
  html += F("    console.error('Error:', error);");
  html += F("    showToast('Error saving settings', true);");
  html += F("  })");
  html += F("  .finally(() => {");
  html += F("    submitButton.disabled = false;");
  html += F("    submitButton.classList.remove('saving');");
  html += F("  });");
  html += F("}");
  html += F("");
  html += F("updateBatteryStatus();");
  html += F("setInterval(updateBatteryStatus, 5000);");
  html += F("</script>");

  html += F("</div>");
  return html;
}

String WiFiManager::generateStatusJson() const {
  String json = "{";
  json += "\"wifiEnabled\":" + String(wifiEnabled ? "true" : "false") + ",";
  json += "\"uptime\":" + String((millis() - startTime) / 1000);
  json += "}";
  return json;
}

void WiFiManager::handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: " + server.uri() + "\n";
  message += "Method: " + String(server.method() == HTTP_GET ? "GET" : "POST") + "\n";
  server.send(404, "text/plain", message);
}

void WiFiManager::handleAPI() { server.send(200, "application/json", generateStatusJson()); }

void WiFiManager::handleBatteryStatus() {
  auto& powerManager = PowerManager::getInstance();
  float voltage = powerManager.getBatteryVoltage();
  float percentage = powerManager.getBatteryPercent();  // Uses LiPo discharge curve
  bool isCharging = powerManager.isUSBPowered();
  
  // Create JSON response
  String json = "{";
  json += "\"voltage\":" + String(voltage, 2) + ",";
  json += "\"percentage\":" + String((int)percentage) + ",";
  json += "\"isCharging\":" + String(isCharging ? "true" : "false");
  json += "}";
  
  server.send(200, "application/json", json);
}

void WiFiManager::handleExportMessages() {
  auto& stationManager = StationManager::getInstance();
  JsonDocument doc;
  JsonArray stations = doc["stations"].to<JsonArray>();

  for (size_t i = 0; i < stationManager.getStationCount(); i++) {
    const Station* station = stationManager.getStation(i);
    if (!station) continue;

    JsonObject stationObj = stations.add<JsonObject>();
    stationObj["index"] = i;
    stationObj["name"] = station->getName();
    stationObj["message"] = station->getMessage();
    stationObj["enabled"] = station->isEnabled();

    // Add wave band as string for readability
    switch (station->getBand()) {
      case WaveBand::LONG_WAVE:
        stationObj["band"] = "LONG_WAVE";
        break;
      case WaveBand::MEDIUM_WAVE:
        stationObj["band"] = "MEDIUM_WAVE";
        break;
      case WaveBand::SHORT_WAVE:
        stationObj["band"] = "SHORT_WAVE";
        break;
    }
  }

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.println("Exported messages: " + response);
#endif
}

void WiFiManager::handleImportMessages() {
  String jsonData = server.arg("plain");

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.println("Received import data: " + jsonData);
#endif

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, jsonData);

  if (error) {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("Failed to parse import JSON");
#endif
    server.send(400, "application/json", "{\"success\":false,\"message\":\"Invalid JSON\"}");
    return;
  }

  auto& stationManager = StationManager::getInstance();
  JsonArray stations = doc["stations"];
  
  if (!stations) {
    server.send(400, "application/json", "{\"success\":false,\"message\":\"No stations array found\"}");
    return;
  }

  bool success = true;
  String message = "Messages imported successfully";
  int importedCount = 0;

  for (JsonObject stationObj : stations) {
    int index = stationObj["index"];
    Station* station = stationManager.getStation(index);
    
    if (station && stationObj["message"].is<const char*>()) {
      const char* newMessage = stationObj["message"];
      station->setMessage(newMessage);
      
      if (stationObj["enabled"].is<bool>()) {
        station->setEnabled(stationObj["enabled"]);
      }
      
      importedCount++;
#ifdef DEBUG_SERIAL_OUTPUT
      Serial.printf("Imported message for station %d: %s\n", index, newMessage);
#endif
    }
  }

  if (importedCount > 0) {
    stationManager.saveToPreferences();
    message = String(importedCount) + " messages imported successfully";
  } else {
    success = false;
    message = "No valid messages found to import";
  }

  JsonDocument response;
  response["success"] = success;
  response["message"] = message;

  String responseStr;
  serializeJson(response, responseStr);
  server.send(200, "application/json", responseStr);
}

void WiFiManager::updateStatusLED() {
  if (wifiEnabled) {
    // Check if any clients are connected
    if (hasConnectedClients()) {
      // Solid LED when clients are connected
      digitalWrite(Pins::SW_LED, HIGH);
    } else {
      // Flash LED when no clients are connected
      unsigned long currentTime = millis();
      if (currentTime - lastLedFlash >= LED_FLASH_INTERVAL) {
        flashLED();
        lastLedFlash = currentTime;
      }
    }
  }
}

void WiFiManager::flashLED() {
  static bool ledState = false;
  ledState = !ledState;
  digitalWrite(Pins::SW_LED, ledState);
}

String WiFiManager::generateHTML(const String& content) const {
  String html;
  html.reserve(8192);  // Reserve space to prevent fragmentation

  // Read from PROGMEM
  html += FPSTR(HTML_HEADER);
  html += FPSTR(CSS_STYLES);
  html += F("</style>");
  html += F("<script>");
  html += FPSTR(JAVASCRIPT_CODE);
  html += F("</script>");
  html += F("</head><body>");
  html += content;
  html += FPSTR(HTML_FOOTER);
  return html;
}
