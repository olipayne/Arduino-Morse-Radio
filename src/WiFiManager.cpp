#include "WiFiManager.h"
#include "Config.h"
#include "Station.h"
#include <vector>
#include <WiFi.h>

namespace WiFiManager
{
  using namespace Config;

  WebServer server(80);

  bool wifiEnabled = false;
  unsigned long wifiStartTime = 0;

  // Function prototypes
  void initWebServer();
  void handleRoot();
  void handleSaveConfig();
  void handleResetConfig();
  void handleNotFound();

  void initWiFiManager()
  {
    WiFi.mode(WIFI_OFF);
  }

  void startWiFi()
  {
    String macAddress = WiFi.macAddress();
    macAddress.replace(":", "");
    String ssid = "Radio_" + macAddress.substring(6);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid.c_str());

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP SSID: ");
    Serial.println(ssid);
    Serial.print("AP IP address: ");
    Serial.println(IP);

    initWebServer();

    server.begin();
    Serial.println("Web server started");

    wifiStartTime = millis();
  }

  void stopWiFi()
  {
    server.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("Web server stopped");
  }

  void handleWiFi()
  {
    server.handleClient();
  }

  void initWebServer()
  {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/save", HTTP_POST, handleSaveConfig);
    server.on("/reset", HTTP_POST, handleResetConfig);
    server.onNotFound(handleNotFound);
  }

  void handleRoot()
  {
    wifiStartTime = millis();
    Serial.println("Wi-Fi timer reset");

    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>Radio Configuration</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
/* CSS styles here */
body {
    font-family: Arial, sans-serif;
    background-color: #f0f0f0;
    margin: 0;
    padding: 0;
}
.container {
    max-width: 480px;
    margin: 0 auto;
    padding: 15px;
}
h1 {
    color: #333;
    text-align: center;
    font-size: 1.5em;
}
form {
    background-color: #fff;
    padding: 15px;
    border-radius: 10px;
    margin-bottom: 20px;
}
label {
    display: block;
    margin-top: 15px;
    font-size: 1em;
}
input[type="text"],
input[type="number"],
select,
input[type="range"] {
    width: 100%;
    padding: 8px;
    margin-top: 5px;
    box-sizing: border-box;
    font-size: 1em;
}
input[type="submit"],
button {
    width: 100%;
    background-color: #4CAF50;
    color: white;
    padding: 10px;
    margin-top: 15px;
    border: none;
    border-radius: 4px;
    font-size: 1em;
    cursor: pointer;
}
input[type="submit"]:hover,
button:hover {
    background-color: #45a049;
}
.slider-value {
    text-align: right;
    font-size: 0.9em;
    color: #555;
}
</style>
<script>
function updateSliderValue(val) {
    document.getElementById('volumeValue').innerText = val;
}
</script>
</head>
<body>
<div class="container">
  <h1>Radio Configuration</h1>
  <form action="/save" method="POST">
)rawliteral";

    // Generate station configurations
    for (size_t i = 0; i < stations.size(); ++i)
    {
      html += "<label for=\"stationName" + String(i) + "\">Station Name:</label>";
      html += "<input type=\"text\" id=\"stationName" + String(i) + "\" name=\"stationName" + String(i) + "\" value=\"" + stations[i].getName() + "\">";

      html += "<label for=\"stationFreq" + String(i) + "\">Station Frequency:</label>";
      html += "<input type=\"number\" id=\"stationFreq" + String(i) + "\" name=\"stationFreq" + String(i) + "\" value=\"" + String(stations[i].getFrequency()) + "\">";

      html += "<label for=\"stationMsg" + String(i) + "\">Station Message:</label>";
      html += "<input type=\"text\" id=\"stationMsg" + String(i) + "\" name=\"stationMsg" + String(i) + "\" value=\"" + stations[i].getMessage() + "\">";
    }

    // Add global configurations
    html += R"rawliteral(
    <label for="volume">Speaker Volume: <span id="volumeValue">)rawliteral" +
            String(speakerDutyCycle) + R"rawliteral(</span></label>
    <input type="range" min="1" max="255" id="volume" name="volume" value=")rawliteral" +
            String(speakerDutyCycle) + R"rawliteral(" oninput="updateSliderValue(this.value)">

    <label for="frequency">Speaker Frequency (Hz):</label>
    <input type="number" id="frequency" name="frequency" value=")rawliteral" +
            String(morseFrequency) + R"rawliteral(" min="100" max="2000">

    <label for="morseSpeed">Morse Code Speed:</label>
    <select id="morseSpeed" name="morseSpeed">
      <option value="0")rawliteral" +
            (morseSpeed == MorseSpeed::SLOW ? " selected" : "") + R"rawliteral(>Slow</option>
      <option value="1")rawliteral" +
            (morseSpeed == MorseSpeed::MEDIUM ? " selected" : "") + R"rawliteral(>Medium</option>
      <option value="2")rawliteral" +
            (morseSpeed == MorseSpeed::FAST ? " selected" : "") + R"rawliteral(>Fast</option>
    </select>

    <input type="submit" value="Save">
  </form>
  <form action="/reset" method="POST">
    <button type="submit">Reset to Defaults</button>
  </form>
</div>
</body>
</html>
)rawliteral";

    server.send(200, "text/html", html);
  }

  void handleSaveConfig()
  {
    wifiStartTime = millis();
    Serial.println("Wi-Fi timer reset");

    // Retrieve global settings
    if (server.hasArg("volume"))
    {
      speakerDutyCycle = server.arg("volume").toInt();
    }
    if (server.hasArg("frequency"))
    {
      morseFrequency = server.arg("frequency").toInt();
    }
    if (server.hasArg("morseSpeed"))
    {
      int speedValue = server.arg("morseSpeed").toInt();
      if (speedValue >= 0 && speedValue <= 2)
      {
        morseSpeed = static_cast<MorseSpeed>(speedValue);
        setMorseSpeed(morseSpeed);
      }
    }

    // Retrieve station settings
    std::vector<Station> newStations;

    // Since we don't have a fixed number of stations, we'll iterate until we can't find more
    for (size_t i = 0;; ++i)
    {
      String indexStr = String(i);

      String nameKey = "stationName" + indexStr;
      String freqKey = "stationFreq" + indexStr;
      String msgKey = "stationMsg" + indexStr;

      if (!server.hasArg(nameKey) || !server.hasArg(freqKey) || !server.hasArg(msgKey))
      {
        break;
      }

      String name = server.arg(nameKey);
      int frequency = server.arg(freqKey).toInt();
      String message = server.arg(msgKey);

      newStations.emplace_back(name, frequency, message);
    }

    // Update stations
    stations = newStations;

    // Save configurations
    saveConfigurations();

    // Send confirmation page with auto-redirect
    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta http-equiv="refresh" content="1; url=/" />
<title>Configuration Saved</title>
<style>
body { font-family: Arial, sans-serif; text-align: center; background-color: #f0f0f0; margin: 0; padding: 0; }
h1 { color: #333; margin-top: 20%; }
</style>
</head>
<body>
  <h1>Configuration Saved!</h1>
  <p>You will be redirected back shortly.</p>
</body>
</html>
)rawliteral";

    server.send(200, "text/html", html);
  }

  void handleResetConfig()
  {
    wifiStartTime = millis();
    Serial.println("Wi-Fi timer reset");

    // Reset configurations to default values
    speakerDutyCycle = 64;
    morseFrequency = 800;
    morseSpeed = MorseSpeed::MEDIUM;
    setMorseSpeed(morseSpeed);

    stations.clear();
    stations.emplace_back("London", 1000, "L");
    stations.emplace_back("Hilversum", 2000, "H");
    stations.emplace_back("Barcelona", 3000, "B");

    // Save configurations
    saveConfigurations();

    // Send confirmation page with auto-redirect
    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta http-equiv="refresh" content="1; url=/" />
<title>Configuration Reset</title>
<style>
body { font-family: Arial, sans-serif; text-align: center; background-color: #f0f0f0; margin: 0; padding: 0; }
h1 { color: #333; margin-top: 20%; }
</style>
</head>
<body>
  <h1>Configuration Reset!</h1>
  <p>All settings have been restored to default values.</p>
  <p>You will be redirected back shortly.</p>
</body>
</html>
)rawliteral";

    server.send(200, "text/html", html);
  }

  void handleNotFound()
  {
    server.send(404, "text/plain", "404: Not found");
  }

  void toggleWiFi()
  {
    if (wifiEnabled)
    {
      stopWiFi();
      wifiEnabled = false;
      Serial.println("Wi-Fi disabled");
    }
    else
    {
      startWiFi();
      wifiEnabled = true;
      wifiStartTime = millis();
      Serial.println("Wi-Fi enabled");
    }
  }
} // namespace WiFiManager
