#include "Config.h"
#include "WiFiManager.h"

const char *WiFiManager::HTML_HEADER = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Radio Configuration</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>)";

const char *WiFiManager::CSS_STYLES = R"(
    body { 
        font-family: Arial, sans-serif; 
        max-width: 800px; 
        margin: 0 auto; 
        padding: 20px;
        background-color: #f0f0f0;
    }
    .station { 
        background: #ffffff; 
        padding: 15px; 
        margin: 10px 0; 
        border-radius: 5px;
        box-shadow: 0 2px 4px rgba(0,0,0,0.1);
    }
    .wave-band { 
        background: #e0e0e0; 
        padding: 10px; 
        margin: 15px 0; 
        border-radius: 3px;
        border-left: 4px solid #2196F3;
    }
    .freq-group { 
        display: flex; 
        align-items: center; 
        gap: 10px; 
        margin: 5px 0; 
    }
    input[type="number"], input[type="text"] { 
        flex: 1; 
        padding: 8px; 
        margin: 5px 0; 
        border: 1px solid #ddd;
        border-radius: 4px;
        box-sizing: border-box;
    }
    button { 
        background: #4CAF50; 
        color: white; 
        padding: 8px 15px; 
        border: none; 
        border-radius: 5px; 
        cursor: pointer;
        transition: background-color 0.3s;
    }
    button:hover { 
        background: #45a049; 
    }
    .tune-button { 
        background: #2196F3;
        min-width: 100px;
    }
    .tune-button:hover { 
        background: #1976D2; 
    }
    .status { 
        color: #666; 
        font-size: 0.9em; 
        margin-top: 5px;
        background: #fff;
        padding: 10px;
        border-radius: 4px;
        box-shadow: 0 2px 4px rgba(0,0,0,0.1);
    }
    h1 {
        color: #333;
        border-bottom: 2px solid #2196F3;
        padding-bottom: 10px;
    }
    h2 {
        color: #444;
        margin-top: 0;
    }
    h3 {
        color: #666;
        margin: 0 0 10px 0;
    }
)";

const char *WiFiManager::HTML_FOOTER = R"(
    </body>
</html>)";

const char *WiFiManager::JAVASCRIPT_CODE = R"(
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

    // Update tuning value every second
    setInterval(updateTuningValue, 1000);
)";

void WiFiManager::begin()
{
    pinMode(Pins::WIFI_BUTTON, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

void WiFiManager::toggle()
{
    if (wifiEnabled)
    {
        stop();
    }
    else
    {
        startAP();
    }
}

void WiFiManager::stop()
{
    if (wifiEnabled)
    {
        server.stop();
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_OFF);
        wifiEnabled = false;
        digitalWrite(LED_BUILTIN, LOW);
        Log::println("WiFi stopped");
    }
}

void WiFiManager::startAP()
{
    WiFi.mode(WIFI_AP);

    String ssid = "Radio_" + String((uint32_t)ESP.getEfuseMac(), HEX);

    if (WiFi.softAP(ssid.c_str(), nullptr, AP_CHANNEL, false, MAX_CONNECTIONS))
    {
        setupServer();
        setupMDNS();

        wifiEnabled = true;
        startTime = millis();

        Log::println("WiFi AP started: ", ssid.c_str());
        Log::println("AP IP address: ", WiFi.softAPIP());
    }
    else
    {
        Log::println("Failed to start WiFi AP");
    }
}

void WiFiManager::handle()
{
    if (!wifiEnabled)
        return;

    server.handleClient();

    if (millis() - startTime > DEFAULT_TIMEOUT)
    {
        stop();
        return;
    }

    updateStatusLED();
}

void WiFiManager::setupServer()
{
    server.on("/", HTTP_GET, [this]()
              { handleRoot(); });
    server.on("/save", HTTP_POST, [this]()
              { handleSaveConfig(); });
    server.on("/tuning", HTTP_GET, [this]()
              { handleGetTuningValue(); });
    server.on("/stations", HTTP_GET, [this]()
              { handleStationConfig(); });
    server.on("/api/status", HTTP_GET, [this]()
              { handleAPI(); });
    server.onNotFound([this]()
                      { handleNotFound(); });
    server.begin();
}

void WiFiManager::setupMDNS()
{
    if (MDNS.begin(hostname.c_str()))
    {
        MDNS.addService("http", "tcp", 80);
        Log::println("MDNS responder started at http://", hostname.c_str(), ".local");
    }
}

void WiFiManager::handleRoot()
{
    server.send(200, "text/html", generateHTML(generateConfigPage()));
}

void WiFiManager::handleGetTuningValue()
{
    int tuningValue = analogRead(Pins::TUNING_POT);
    String response = "{\"value\":" + String(tuningValue) + "}";
    server.send(200, "application/json", response);
}

void WiFiManager::handleSaveConfig()
{
    bool success = true;
    String message = "Configuration saved successfully";

    auto &stationManager = StationManager::getInstance();

    for (size_t i = 0; i < stationManager.getStationCount(); i++)
    {
        String freqKey = "freq_" + String(i);
        String msgKey = "msg_" + String(i);

        if (server.hasArg(freqKey) && server.hasArg(msgKey))
        {
            int frequency = server.arg(freqKey).toInt();
            String newMessage = server.arg(msgKey);

            if (frequency > 0)
            {
                stationManager.updateStation(i, frequency, newMessage);
            }
            else
            {
                success = false;
                message = "Invalid frequency value";
                break;
            }
        }
    }

    if (success)
    {
        stationManager.saveToPreferences();
    }

    String response = "<div class='" + String(success ? "success" : "error") + "'>" +
                      message + "</div>";
    server.send(200, "text/html", generateHTML(response + generateConfigPage()));
}

String WiFiManager::generateConfigPage() const
{
    String html = "<h1>Radio Configuration</h1>";
    html += "<form method='POST' action='/save'>";
    html += generateStationTable();
    html += "<button type='submit'>Save Configuration</button>";
    html += "</form>";
    return html;
}

String WiFiManager::generateStationTable() const
{
    String html;
    auto &stationManager = StationManager::getInstance();

    html += "<div class='status'>Current Tuning Value: <span id='currentTuning'>-</span></div>";

    WaveBand currentBand = WaveBand::LONG_WAVE;
    bool firstBand = true;

    for (size_t i = 0; i < stationManager.getStationCount(); i++)
    {
        const Station *station = stationManager.getStation(i);
        if (!station)
            continue;

        if (station->getBand() != currentBand)
        {
            if (!firstBand)
                html += "</div>";
            currentBand = station->getBand();
            html += "<div class='wave-band'><h2>" + String(toString(currentBand)) + "</h2>";
            firstBand = false;
        }

        html += "<div class='station'>";
        html += "<h3>" + String(station->getName()) + "</h3>";

        html += "<div class='freq-group'>";
        html += "<input type='number' id='freq_" + String(i) + "' name='freq_" + String(i) +
                "' value='" + String(station->getFrequency()) + "'>";
        html += "<button type='button' class='tune-button' " +
                String("onclick='setFrequency(\"freq_") + String(i) + "\")'>Set Current</button>";
        html += "</div>";

        html += "<label>Message:<br><input type='text' name='msg_" + String(i) +
                "' value='" + station->getMessage() + "'></label>";
        html += "</div>";
    }

    if (!firstBand)
        html += "</div>";
    return html;
}

String WiFiManager::generateStatusJson() const
{
    String json = "{";
    json += "\"wifiEnabled\":" + String(wifiEnabled ? "true" : "false") + ",";
    json += "\"uptime\":" + String((millis() - startTime) / 1000) + ",";
    json += "\"timeoutIn\":" + String((DEFAULT_TIMEOUT - (millis() - startTime)) / 1000);
    json += "}";
    return json;
}

void WiFiManager::handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: " + server.uri() + "\n";
    message += "Method: " + String(server.method() == HTTP_GET ? "GET" : "POST") + "\n";
    server.send(404, "text/plain", message);
}

void WiFiManager::handleStationConfig()
{
    server.send(200, "text/html", generateHTML(generateStationTable()));
}

void WiFiManager::handleAPI()
{
    server.send(200, "application/json", generateStatusJson());
}

void WiFiManager::updateStatusLED()
{
    if (wifiEnabled)
    {
        unsigned long currentTime = millis();
        if (currentTime - lastLedFlash >= LED_FLASH_INTERVAL)
        {
            flashLED();
            lastLedFlash = currentTime;
        }
    }
}

void WiFiManager::flashLED()
{
    static bool ledState = false;
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
}

String WiFiManager::generateHTML(const String &content) const
{
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
