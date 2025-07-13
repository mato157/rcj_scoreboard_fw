#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "ws2812b_control.h"
#include "team.h"
#include "net.h"
#include "button.h"
#include "definitions.h"




uint32_t button_start_time = 0; // Variable to check if long press is detected


void setup() {
  Serial.begin(115200); // Initialize serial communication for debugging

  led_init(); // Initialize the WS2812B LED strip

  led_test(); // Test the LED strip with a simple color cycle

  init_button(); // Initialize the button pin

  // for (int i = 0; i < 10; i++) {
  //   team1.setScore(i+8); // Set score for team1
  //   team2.setScore(i+12); // Set score for team2
  //   delay(1000); // Wait for a second to visualize the change
  // }

  //led_digit_test(); // Test the digit display functionality



  connectToWiFi(); // Connect to WiFi or start AP mode if connection fails

  // connect to MQTT broker
  setupMQTT(); // Set up MQTT connection
}

void loop() {
  // // Handle client requests if in AP mode
  // //if (WiFi.getMode() == WIFI_AP) {
  //   handleClient(); // Handle incoming client requests
  // //}
  // // put your main code here, to run repeatedly:

  handleWiFi(); // Check and maintain WiFi connection
  handleMQTT(); // Check and maintain MQTT connection
  handle_button(); // Handle button presses for AP mode activation

  
}










// #include <WiFi.h>
// #include <WebServer.h>
// #include <Preferences.h>

// #define BUTTON_PIN 0  // napr. GPIO0
// #define LED_PIN 2     // voliteľne na indikáciu

// WebServer server(80);
// Preferences preferences;

// String ssid = "";
// String pass = "";
// String mqttTopic = "";

// bool shouldStartConfig = false;

// void handleRoot() {
//   String html = R"rawliteral(
//     <!DOCTYPE html><html>
//     <head><title>ESP Config</title></head>
//     <body>
//       <h2>WiFi & MQTT Setup</h2>
//       <form action="/save" method="POST">
//         SSID: <input type="text" name="ssid"><br>
//         Password: <input type="password" name="pass"><br>
//         MQTT Topic: <input type="text" name="topic"><br><br>
//         <input type="submit" value="Save and Restart">
//       </form>
//     </body></html>
//   )rawliteral";
//   server.send(200, "text/html", html);
// }

// void handleSave() {
//   ssid = server.arg("ssid");
//   pass = server.arg("pass");
//   mqttTopic = server.arg("topic");

//   preferences.begin("config", false);
//   preferences.putString("ssid", ssid);
//   preferences.putString("pass", pass);
//   preferences.putString("topic", mqttTopic);
//   preferences.end();

//   server.send(200, "text/html", "<h2>Saved! Rebooting...</h2>");
//   delay(2000);
//   ESP.restart();
// }

// void startAPMode() {
//   Serial.println("Starting AP mode...");
//   WiFi.softAP("ESP_Config");
//   IPAddress IP = WiFi.softAPIP();
//   Serial.print("AP IP address: ");
//   Serial.println(IP);

//   server.on("/", handleRoot);
//   server.on("/save", HTTP_POST, handleSave);
//   server.begin();
// }

// void connectToWiFi() {
//   preferences.begin("config", true);
//   ssid = preferences.getString("ssid", "");
//   pass = preferences.getString("pass", "");
//   mqttTopic = preferences.getString("topic", "");
//   preferences.end();

//   Serial.printf("Connecting to %s\n", ssid.c_str());
//   WiFi.begin(ssid.c_str(), pass.c_str());

//   int maxTries = 20;
//   while (WiFi.status() != WL_CONNECTED && maxTries-- > 0) {
//     delay(500);
//     Serial.print(".");
//   }

//   if (WiFi.status() == WL_CONNECTED) {
//     Serial.println("\nWiFi connected.");
//     Serial.print("IP address: ");
//     Serial.println(WiFi.localIP());
//     // MQTT logic will go here
//   } else {
//     Serial.println("\nFailed to connect. Starting AP mode.");
//     startAPMode();
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   pinMode(BUTTON_PIN, INPUT_PULLUP);
//   pinMode(LED_PIN, OUTPUT);

//   // Check if button is pressed for 5 seconds
//   unsigned long pressStart = 0;
//   while (digitalRead(BUTTON_PIN) == LOW) {
//     if (pressStart == 0) pressStart = millis();
//     if (millis() - pressStart >= 5000) {
//       shouldStartConfig = true;
//       break;
//     }
//     delay(100);
//   }

//   if (shouldStartConfig) {
//     startAPMode();
//   } else {
//     connectToWiFi();
//   }
// }

// void loop() {
//   if (shouldStartConfig) {
//     server.handleClient();
//   }
// }

