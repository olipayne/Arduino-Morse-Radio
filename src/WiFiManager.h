// WiFiManager.h
#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

class WiFiManager
{
public:
    WiFiManager(int ledPin, int buttonPin);

    void init();
    void handle();
    void updateButton();
    void stop();
    bool isWiFiEnabled() const;

private:
    void startWiFi();
    void initWebServer();
    void handleRoot();
    void handleSaveConfig();
    void handleResetConfig();
    void handleNotFound();

    WebServer server;
    bool wifiEnabled;
    unsigned long wifiStartTime;
    unsigned long lastButtonPress;
    int ledPin;
    int buttonPin;
};

#endif
