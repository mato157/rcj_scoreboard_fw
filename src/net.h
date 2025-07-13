#ifndef NET_H
#define NET_H

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

// Function declarations
void handleRoot();
void handleSave();
void startAPMode();
void connectToWiFi();
//void handleClient();

void setupMQTT();
void handleWiFi();
void handleMQTT();

#endif // NET_H