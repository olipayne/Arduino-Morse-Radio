#include "WiFiManager.h"

// Web server and DNS server objects
WebServer server(80);
DNSServer dnsServer;

const byte DNS_PORT = 53;

// Function prototypes
void initWebServer();
void handleRoot();
void handleSaveConfig();
void handleResetConfig();
void handleNotFound();

// Extern variables
extern bool wifiEnabled;
extern const int blueLEDPin; // Access blue LED pin

// Initializes Wi-Fi manager (Wi-Fi is off by default)
void initWiFiManager()
{
  WiFi.mode(WIFI_OFF);
}

// Starts Wi-Fi and web server
void startWiFi()
{
  // Get the MAC address
  String macAddress = WiFi.macAddress();
  // Remove colons from MAC address for SSID
  macAddress.replace(":", "");

  // Create a unique SSID using the MAC address
  String ssid = "Radio_" + macAddress.substring(6); // Use last 6 characters

  // Start Wi-Fi in Access Point mode
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid.c_str());

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP SSID: ");
  Serial.println(ssid);
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Setup DNS server for captive portal
  dnsServer.start(DNS_PORT, "*", IP);

  // Initialize web server routes
  initWebServer();

  // Start web server
  server.begin();
  Serial.println("Web server started");
}

// Stops Wi-Fi and web server
void stopWiFi()
{
  server.stop();
  dnsServer.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.println("Web server stopped");
}

// Handles Wi-Fi tasks
void handleWiFi()
{
  dnsServer.processNextRequest();
  server.handleClient();
}

// Initializes web server routes
void initWebServer()
{
  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSaveConfig);
  server.on("/reset", HTTP_POST, handleResetConfig);
  server.onNotFound(handleNotFound);
}

// Handles the root URL (configuration page)
void handleRoot()
{
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>Radio Configuration</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
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
    <label for="londonMessage">London Message:</label>
    <input type="text" id="londonMessage" name="londonMessage" value="%londonMessage%" maxlength="20" pattern="[A-Za-z0-9]*">

    <label for="hilversumMessage">Hilversum Message:</label>
    <input type="text" id="hilversumMessage" name="hilversumMessage" value="%hilversumMessage%" maxlength="20" pattern="[A-Za-z0-9]*">

    <label for="barcelonaMessage">Barcelona Message:</label>
    <input type="text" id="barcelonaMessage" name="barcelonaMessage" value="%barcelonaMessage%" maxlength="20" pattern="[A-Za-z0-9]*">

    <label for="volume">Speaker Volume: <span id="volumeValue">%volume%</span></label>
    <input type="range" min="1" max="255" id="volume" name="volume" value="%volume%" oninput="updateSliderValue(this.value)">

    <label for="frequency">Speaker Frequency (Hz):</label>
    <input type="number" id="frequency" name="frequency" value="%frequency%" min="100" max="2000">

    <label for="morseSpeed">Morse Code Speed:</label>
    <select id="morseSpeed" name="morseSpeed">
      <option value="0"%lowSpeedSelected%>Low</option>
      <option value="1"%mediumSpeedSelected%>Medium</option>
      <option value="2"%highSpeedSelected%>High</option>
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

  // Replace placeholders with current values
  html.replace("%londonMessage%", londonMessage);
  html.replace("%hilversumMessage%", hilversumMessage);
  html.replace("%barcelonaMessage%", barcelonaMessage);
  html.replace("%volume%", String(speakerDutyCycle));
  html.replace("%frequency%", String(morseFrequency));

  // Set the selected option for Morse code speed
  String lowSpeedSelected = "";
  String mediumSpeedSelected = "";
  String highSpeedSelected = "";

  if (morseSpeed == LOW_SPEED)
  {
    lowSpeedSelected = " selected";
  }
  else if (morseSpeed == MEDIUM_SPEED)
  {
    mediumSpeedSelected = " selected";
  }
  else if (morseSpeed == HIGH_SPEED)
  {
    highSpeedSelected = " selected";
  }

  html.replace("%lowSpeedSelected%", lowSpeedSelected);
  html.replace("%mediumSpeedSelected%", mediumSpeedSelected);
  html.replace("%highSpeedSelected%", highSpeedSelected);

  // Send the HTML response
  server.send(200, "text/html", html);
}

// Handles saving configurations
void handleSaveConfig()
{
  // Retrieve form data
  if (server.hasArg("londonMessage"))
  {
    londonMessage = server.arg("londonMessage");
  }
  if (server.hasArg("hilversumMessage"))
  {
    hilversumMessage = server.arg("hilversumMessage");
  }
  if (server.hasArg("barcelonaMessage"))
  {
    barcelonaMessage = server.arg("barcelonaMessage");
  }
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

// Handles resetting configurations to defaults
void handleResetConfig()
{
  // Reset configurations to default values
  londonMessage = "L";
  hilversumMessage = "H";
  barcelonaMessage = "B";
  speakerDutyCycle = 64;
  morseFrequency = 500;
  morseSpeed = MEDIUM_SPEED;

  // Update Morse code timing
  setMorseSpeed(morseSpeed);

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

// Handles not found pages (404)
void handleNotFound()
{
  server.send(404, "text/plain", "404: Not found");
}

void toggleWiFi()
{
  wifiEnabled = !wifiEnabled;

  if (wifiEnabled)
  {
    Serial.println("Turning Wi-Fi ON");
    startWiFi();
    digitalWrite(blueLEDPin, LOW); // Active-low: LOW turns LED ON
  }
  else
  {
    Serial.println("Turning Wi-Fi OFF");
    stopWiFi();
    digitalWrite(blueLEDPin, HIGH); // Active-low: HIGH turns LED OFF
  }
}
