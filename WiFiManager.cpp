#include "WiFiManager.h"

// Web server and DNS server objects
WebServer server(80);
DNSServer dnsServer;

const byte DNS_PORT = 53;

void initWiFiManager() {
  // Start Wi-Fi in AP mode
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP32_Config_AP");  // Set the SSID (add password if desired)

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Setup DNS server to redirect all requests to the ESP32
  dnsServer.start(DNS_PORT, "*", IP);

  // Initialize the web server routes
  initWebServer();

  // Start the server
  server.begin();
  Serial.println("Web server started");
}

void handleWiFi() {
  dnsServer.processNextRequest();
  server.handleClient();
}

// Web Server Handlers
void initWebServer() {
  // Serve the configuration page at the root URL
  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSaveConfig);

  // Handle not found pages
  server.onNotFound(handleNotFound);
}

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE HTML><html>
  <head>
    <title>ESP32 Configuration</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { font-family: Arial, sans-serif; text-align: center; background-color: #f0f0f0; }
      h1 { color: #333; }
      form { display: inline-block; background-color: #fff; padding: 20px; border-radius: 10px; max-width: 400px; }
      input[type="text"], input[type="number"], select {
        width: 100%; padding: 8px; margin: 5px 0; box-sizing: border-box;
      }
      input[type="submit"] {
        background-color: #4CAF50; color: white; padding: 10px 20px;
        border: none; border-radius: 4px; cursor: pointer;
      }
      input[type="submit"]:hover {
        background-color: #45a049;
      }
      .slider {
        width: 100%;
      }
      label {
        display: block; text-align: left; margin-top: 10px;
      }
    </style>
  </head>
  <body>
    <h1>ESP32 Configuration</h1>
    <form action="/save" method="POST">
      <label for="londonMessage">London Message:</label>
      <input type="text" id="londonMessage" name="londonMessage" value="%londonMessage%">
      
      <label for="hilversumMessage">Hilversum Message:</label>
      <input type="text" id="hilversumMessage" name="hilversumMessage" value="%hilversumMessage%">
      
      <label for="barcelonaMessage">Barcelona Message:</label>
      <input type="text" id="barcelonaMessage" name="barcelonaMessage" value="%barcelonaMessage%">
      
      <label for="volume">Speaker Volume:</label>
      <input type="range" min="0" max="255" class="slider" id="volume" name="volume" value="%volume%">
      
      <label for="frequency">Speaker Frequency (Hz):</label>
      <input type="number" id="frequency" name="frequency" value="%frequency%">
      
      <label for="morseSpeed">Morse Code Speed:</label>
      <select id="morseSpeed" name="morseSpeed">
        <option value="0"%lowSpeedSelected%>Low</option>
        <option value="1"%mediumSpeedSelected%>Medium</option>
        <option value="2"%highSpeedSelected%>High</option>
      </select>
      
      <br><br>
      <input type="submit" value="Save">
    </form>
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

  if (morseSpeed == LOW_SPEED) {
    lowSpeedSelected = " selected";
  } else if (morseSpeed == MEDIUM_SPEED) {
    mediumSpeedSelected = " selected";
  } else if (morseSpeed == HIGH_SPEED) {
    highSpeedSelected = " selected";
  }

  html.replace("%lowSpeedSelected%", lowSpeedSelected);
  html.replace("%mediumSpeedSelected%", mediumSpeedSelected);
  html.replace("%highSpeedSelected%", highSpeedSelected);

  server.send(200, "text/html", html);
}

void handleSaveConfig() {
  // Retrieve form data
  if (server.hasArg("londonMessage")) {
    londonMessage = server.arg("londonMessage");
  }
  if (server.hasArg("hilversumMessage")) {
    hilversumMessage = server.arg("hilversumMessage");
  }
  if (server.hasArg("barcelonaMessage")) {
    barcelonaMessage = server.arg("barcelonaMessage");
  }
  if (server.hasArg("volume")) {
    speakerDutyCycle = server.arg("volume").toInt();
  }
  if (server.hasArg("frequency")) {
    morseFrequency = server.arg("frequency").toInt();
  }
  if (server.hasArg("morseSpeed")) {
    int speedValue = server.arg("morseSpeed").toInt();
    if (speedValue >= 0 && speedValue <= 2) {
      morseSpeed = static_cast<MorseSpeed>(speedValue);
      setMorseSpeed(morseSpeed);
    }
  }

  // Save configurations to non-volatile storage
  saveConfigurations();

  // Send confirmation page
  server.send(200, "text/html", "<html><body><h1>Configuration Saved!</h1><p>The device will now restart.</p></body></html>");

  // Small delay to allow the message to be sent
  delay(1000);

  // Restart the ESP32 to apply changes
  ESP.restart();
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}
