// WiFiManager.cpp
#include "WiFiManager.h"

// Include your configuration variables and functions
extern String londonMessage;
extern String hilversumMessage;
extern String barcelonaMessage;
extern unsigned int speakerDutyCycle;
extern unsigned int morseFrequency;
extern unsigned int morseSpeed;
extern void saveConfigurations();
extern void loadConfigurations();
extern void setMorseSpeed(unsigned int dotDuration);

WiFiManager::WiFiManager(int ledPin, int buttonPin)
    : server(80)
{
  wifiEnabled = false;
  wifiStartTime = 0;
  lastButtonPress = 0;
  this->ledPin = ledPin;
  this->buttonPin = buttonPin;
}

void WiFiManager::init()
{
  WiFi.mode(WIFI_OFF);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Turn off LED
  pinMode(buttonPin, INPUT_PULLUP);
}

void WiFiManager::handle()
{
  if (wifiEnabled)
  {
    server.handleClient();

    // Flash LED while Wi-Fi is active
    static unsigned long ledFlashStartTime = 0;
    static unsigned long ledFlashInterval = 500;
    static bool ledState = LOW;

    unsigned long currentTime = millis();

    if (currentTime - ledFlashStartTime >= ledFlashInterval)
    {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      ledFlashStartTime = currentTime;
    }

    // Turn off Wi-Fi after 2 minutes
    if (currentTime - wifiStartTime >= 120000)
    {
      stop();
    }
  }
  else
  {
    digitalWrite(ledPin, LOW); // Ensure LED is off
  }
}

void WiFiManager::updateButton()
{
  if (digitalRead(buttonPin) == LOW)
  {
    unsigned long currentTime = millis();
    if (currentTime - lastButtonPress > 500) // Debounce
    {
      if (wifiEnabled)
      {
        stop();
      }
      else
      {
        startWiFi();
      }
      lastButtonPress = currentTime;
    }
  }
}

void WiFiManager::startWiFi()
{
  // Get the MAC address
  String macAddress = WiFi.macAddress();
  macAddress.replace(":", "");

  // Create a unique SSID using the MAC address
  String ssid = "Radio_" + macAddress.substring(6);

  // Start Wi-Fi in Access Point mode
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid.c_str());

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP SSID: ");
  Serial.println(ssid);
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Initialize web server routes
  initWebServer();

  // Start web server
  server.begin();
  Serial.println("Web server started");

  // Record the start time for Wi-Fi
  wifiStartTime = millis();
  wifiEnabled = true;
}

void WiFiManager::stop()
{
  // Stop the web server first
  server.close(); // Close the server
  server.stop();
  Serial.println("Web server stopped");

  // Disconnect clients and stop the AP
  WiFi.softAPdisconnect(true); // Disconnect all clients and shut down the AP
  Serial.println("Wi-Fi Soft AP disconnected");

  // Wait for the Wi-Fi stack to process the disconnection
  delay(100);

  // Disable Wi-Fi and release resources
  WiFi.disconnect(true, true); // Disconnect and reset the Wi-Fi driver
  WiFi.mode(WIFI_OFF);
  Serial.println("Wi-Fi mode set to WIFI_OFF and Wi-Fi disconnected");

  // Update Wi-Fi state
  wifiEnabled = false;

  // Turn off the Wi-Fi status LED
  digitalWrite(ledPin, LOW);
}

void WiFiManager::initWebServer()
{
  server.on("/", HTTP_GET, std::bind(&WiFiManager::handleRoot, this));
  server.on("/save", HTTP_POST, std::bind(&WiFiManager::handleSaveConfig, this));
  server.on("/reset", HTTP_POST, std::bind(&WiFiManager::handleResetConfig, this));
  server.onNotFound(std::bind(&WiFiManager::handleNotFound, this));
}

void WiFiManager::handleRoot()
{
  // Reset the Wi-Fi timer
  wifiStartTime = millis();

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

  if (morseSpeed == 0)
  {
    lowSpeedSelected = " selected";
  }
  else if (morseSpeed == 1)
  {
    mediumSpeedSelected = " selected";
  }
  else if (morseSpeed == 2)
  {
    highSpeedSelected = " selected";
  }

  html.replace("%lowSpeedSelected%", lowSpeedSelected);
  html.replace("%mediumSpeedSelected%", mediumSpeedSelected);
  html.replace("%highSpeedSelected%", highSpeedSelected);

  // Send the HTML response
  server.send(200, "text/html", html);
}

void WiFiManager::handleSaveConfig()
{
  // Reset the Wi-Fi timer
  wifiStartTime = millis();

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
      morseSpeed = speedValue;
      unsigned int dotDuration;
      switch (morseSpeed)
      {
      case 0:
        dotDuration = 500;
        break;
      case 1:
        dotDuration = 300;
        break;
      case 2:
        dotDuration = 100;
        break;
      }
      setMorseSpeed(dotDuration);
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

void WiFiManager::handleResetConfig()
{
  // Reset the Wi-Fi timer
  wifiStartTime = millis();

  // Reset configurations to default values
  londonMessage = "L";
  hilversumMessage = "H";
  barcelonaMessage = "B";
  speakerDutyCycle = 64;
  morseFrequency = 800;
  morseSpeed = 1; // Medium speed

  // Update Morse code timing
  setMorseSpeed(300);

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

void WiFiManager::handleNotFound()
{
  server.send(404, "text/plain", "404: Not found");
}

bool WiFiManager::isWiFiEnabled() const
{
  return wifiEnabled;
}
