#include "WiFiManager.h"
#include <WiFi.h>
#include <WebServer.h>
#include "Config.h"
#include "Stations.h"

WebServer server(80);
bool wifiEnabled = false; // Define Wi-Fi state here
unsigned long wifiStartTime = 0;
unsigned long ledFlashStartTime = 0;     // LED flashing timer
const int LED_FLASH_INTERVAL = 500;      // Flash interval for LED
const int LED_BUILTIN_PIN = LED_BUILTIN; // Onboard LED pin

// Function prototypes
void handleRoot();       // Prototype for the root handler
void handleSaveConfig(); // Prototype for the save configuration handler

void initWiFiManager()
{
  WiFi.mode(WIFI_OFF);
  pinMode(LED_BUILTIN_PIN, OUTPUT);   // Set LED as output
  digitalWrite(LED_BUILTIN_PIN, LOW); // Ensure LED is off initially
}

void startWiFi()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Radio_" + WiFi.macAddress().substring(6));
  server.begin();
  wifiEnabled = true;
  wifiStartTime = millis();
  Serial.println("Wi-Fi started");
  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSaveConfig);
}

void stopWiFi()
{
  server.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  wifiEnabled = false;
  digitalWrite(LED_BUILTIN_PIN, LOW); // Turn off LED when Wi-Fi stops
  Serial.println("Wi-Fi stopped");
}

void toggleWiFi()
{
  if (wifiEnabled)
  {
    stopWiFi();
  }
  else
  {
    startWiFi();
  }
}

void handleWiFi()
{
  server.handleClient();
  if (millis() - wifiStartTime > 120000)
  {
    stopWiFi();
  }
}

// Flash the built-in LED while Wi-Fi is active
void updateWiFiLED()
{
  if (wifiEnabled)
  {
    unsigned long currentTime = millis();
    if (currentTime - ledFlashStartTime >= LED_FLASH_INTERVAL)
    {
      digitalWrite(LED_BUILTIN_PIN, !digitalRead(LED_BUILTIN_PIN)); // Toggle LED state
      ledFlashStartTime = currentTime;                              // Reset flash timer
    }
  }
  else
  {
    digitalWrite(LED_BUILTIN_PIN, LOW); // Ensure LED is off when Wi-Fi is inactive
  }
}

// HTML form to configure station frequencies and messages
void handleRoot()
{
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head><title>Radio Configuration</title></head>
<body>
  <h1>Station Configuration</h1>
  <form action="/save" method="POST">
)rawliteral";

  for (int i = 0; i < numStations; i++)
  {
    html += "<h2>" + String(stations[i].name) + " (" + String(stations[i].band == LONG_WAVE ? "Long Wave" : (stations[i].band == MEDIUM_WAVE ? "Medium Wave" : "Short Wave")) + ")</h2>";
    html += "Frequency: <input type='number' name='freq" + String(i) + "' value='" + String(stations[i].frequency) + "'><br>";
    html += "Message: <input type='text' name='msg" + String(i) + "' value='" + stations[i].message + "'><br><br>";
  }

  html += "<input type='submit' value='Save Configurations'>";
  html += "</form></body></html>";

  server.send(200, "text/html", html);
}

// Save configurations from the web form
void handleSaveConfig()
{
  for (int i = 0; i < numStations; i++)
  {
    if (server.hasArg("freq" + String(i)))
    {
      stations[i].frequency = server.arg("freq" + String(i)).toInt();
    }
    if (server.hasArg("msg" + String(i)))
    {
      stations[i].message = server.arg("msg" + String(i));
    }
  }

  // Provide feedback and redirect back to the root page
  String html = "<html><body><h2>Configurations Saved!</h2><a href='/'>Go Back</a></body></html>";
  server.send(200, "text/html", html);
}
