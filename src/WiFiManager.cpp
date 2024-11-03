#include "WiFiManager.h"

// HTML Templates
const char *WiFiManager::HTML_HEADER = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Radio Configuration</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>)";

const char *WiFiManager::CSS_STYLES = R"(
        body { font-family: Arial, sans-serif; max-width: 800px; margin: 0 auto; padding: 20px; }
        .station { background: #f5f5f5; padding: 15px; margin: 10px 0; border-radius: 5px; }
        .wave-band { background: #e0e0e0; padding: 10px; margin: 15px 0; border-radius: 3px; }
        input[type="number"], input[type="text"] { width: 100%; padding: 8px; margin: 5px 0; box-sizing: border-box; }
        button { background: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; }
        button:hover { background: #45a049; }
        .status { color: #666; font-size: 0.9em; margin-top: 5px; }
        .error { color: #ff0000; }
        .success { color: #008000; }
)";

const char *WiFiManager::HTML_FOOTER = R"(
    </body>
</html>)";

WiFiManager::WiFiManager()
    : server(80),
      wifiEnabled(false),
      startTime(0),
      lastLedFlash(0),
      timeoutDuration(DEFAULT_TIMEOUT),
      hostname("radio-config")
{
}

String WiFiManager::generateHTML(const String &content) const
{
  String html = HTML_HEADER;
  html += CSS_STYLES;
  html += "</style></head><body>";
  html += content;
  html += HTML_FOOTER;
  return html;
}

void WiFiManager::begin()
{
  pinMode(Pins::WIFI_BUTTON, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void WiFiManager::handle()
{
  if (!wifiEnabled)
    return;

  server.handleClient();

  // Check for timeout
  if (millis() - startTime > timeoutDuration)
  {
    stop();
    return;
  }

  updateStatusLED();
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
    Serial.println("WiFi stopped");
  }
}

void WiFiManager::startAP()
{
  WiFi.mode(WIFI_AP);

  // Create unique SSID using chip ID
  String ssid = "Radio_" + String((uint32_t)ESP.getEfuseMac(), HEX);

  // Start AP with generated SSID
  if (WiFi.softAP(ssid.c_str(), nullptr, AP_CHANNEL, false, MAX_CONNECTIONS))
  {
    setupServer();
    setupMDNS();

    wifiEnabled = true;
    startTime = millis();
    Serial.println("WiFi AP started: " + ssid);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  }
  else
  {
    Serial.println("Failed to start WiFi AP");
  }
}

void WiFiManager::setupServer()
{
  server.on("/", HTTP_GET, [this]()
            { handleRoot(); });
  server.on("/save", HTTP_POST, [this]()
            { handleSaveConfig(); });
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
    Serial.println("MDNS responder started at http://" + hostname + ".local");
  }
}

void WiFiManager::handleRoot()
{
  server.send(200, "text/html", generateHTML(generateConfigPage()));
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

  String response = "<div class='" + String(success ? "success" : "error") + "'>" + message + "</div>";
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
    html += "<label>Frequency:<br><input type='number' name='freq_" + String(i) +
            "' value='" + String(station->getFrequency()) + "'></label><br>";
    html += "<label>Message:<br><input type='text' name='msg_" + String(i) +
            "' value='" + station->getMessage() + "'></label>";
    html += "</div>";
  }

  if (!firstBand)
    html += "</div>"; // Close last wave-band div
  return html;
}

void WiFiManager::handleStationConfig()
{
  server.send(200, "text/html", generateHTML(generateStationTable()));
}

void WiFiManager::handleAPI()
{
  server.send(200, "application/json", generateStatusJson());
}

String WiFiManager::generateStatusJson() const
{
  String json = "{";
  json += "\"wifiEnabled\":" + String(wifiEnabled ? "true" : "false") + ",";
  json += "\"uptime\":" + String((millis() - startTime) / 1000) + ",";
  json += "\"timeoutIn\":" + String((timeoutDuration - (millis() - startTime)) / 1000);
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

void WiFiManager::setHostname(const String &name)
{
  hostname = name;
  if (wifiEnabled)
  {
    setupMDNS(); // Restart MDNS with new hostname
  }
}