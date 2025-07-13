#include <WiFi.h>
#include <WiFiClientSecure.h> // Include the secure client library
#include <WebServer.h>
#include <PubSubClient.h>
#include <Preferences.h>

#include "net.h"
#include "ws2812b_control.h"
#include "team.h"
#include "button.h"
#include "definitions.h"

WebServer server(80);
Preferences preferences;

String ssid = DEFAULT_WIFI_SSID; // Default SSID
String pass = DEFAULT_WIFI_PASS; // Default password
String mqttFieldNum = DEFAULT_FIELD_NUM; // Default field number



WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);
String mqttTopic = "rcj_soccer/field_"; // base topic for MQTT messages
String mqttTeam1Score = ""; // MQTT topic for team 1 score
String mqttTeam1ID = "";
String mqttTeam2Score = ""; // MQTT topic for team 2 score
String mqttTeam2ID = "";    




void handleRoot() {
    String html = R"rawliteral(
        <!DOCTYPE html><html>
        <head>
            <title>Scoreboard Config</title>
            <style>
                body {
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    height: 100vh;
                    margin: 0;
                    font-family: Arial, sans-serif;
                    background-color: #f0f0f0;
                }
                .container {
                    text-align: center;
                    background: white;
                    padding: 20px;
                    border-radius: 10px;
                    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
                    max-width: 90%;
                    width: 400px;
                }
                input[type="text"], input[type="password"] {
                    width: 100%;
                    padding: 10px;
                    margin: 10px 0;
                    border: 1px solid #ccc;
                    border-radius: 5px;
                }
                input[type="submit"] {
                    background-color: #4CAF50;
                    color: white;
                    padding: 10px 20px;
                    border: none;
                    border-radius: 5px;
                    cursor: pointer;
                }
                input[type="submit"]:hover {
                    background-color: #45a049;
                }
            </style>
        </head>
        <body>
            <div class="container">
                <h2>Scoreboard Config</h2>
                <form action="/save" method="POST">
                    SSID: <input type="text" name="ssid" value=")rawliteral" + ssid + R"rawliteral("><br>
                    Password: <input type="password" name="pass" value=")rawliteral" + pass + R"rawliteral("><br>
                    Field Number: <input type="text" name="topic" value=")rawliteral" + mqttFieldNum + R"rawliteral("><br><br>
                    <input type="submit" value="Save and Restart">
                </form>
            </div>
        </body>
        </html>
    )rawliteral";
    server.send(200, "text/html", html);
}


void handleSave() {
    ssid = server.arg("ssid");
    pass = server.arg("pass");
    mqttFieldNum = server.arg("topic");

    preferences.begin("config", false);
    preferences.putString("ssid", ssid);
    preferences.putString("pass", pass);
    preferences.putString("topic", mqttFieldNum);
    preferences.end();

    server.send(200, "text/html", "<h2>Saved! Rebooting...</h2>");
    delay(2000);
    ESP.restart();
}

void handleClient() {
    while (1) {
        server.handleClient();
    } 
}

void startAPMode() {
    uint8_t mac[6];
    WiFi.softAPmacAddress(mac);
    uint8_t last_mac = mac[5] % 100; // Ensure the last byte is in the range 0-99
    String apName = "ScoreBoard_" + String(last_mac); // Use the last byte of the MAC address for uniqueness

    
    WiFi.softAP(apName.c_str());
    Serial.println("Starting AP mode...");
    
    WiFi.softAP(apName.c_str(), "12345678"); // Default password for AP mode
    Serial.printf("AP Name: %s, Password: 12345678\n", apName.c_str());
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    led_display_word_AP(last_mac); // Display "AP" on the LED strip 

    server.on("/", handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.begin();

    handleClient(); // Start handling client requests
}

void connectToWiFi() {
    bool dash = true; // Set to true to show dash in the display
    preferences.begin("config", true);
    ssid = preferences.getString("ssid", DEFAULT_WIFI_SSID);
    pass = preferences.getString("pass", DEFAULT_WIFI_PASS);
    mqttFieldNum = preferences.getString("topic", DEFAULT_FIELD_NUM);
    preferences.end();

    Serial.printf("Connecting to %s\n", ssid.c_str());
    WiFi.begin(ssid.c_str(), pass.c_str());

    int maxTries = 40;
    while (WiFi.status() != WL_CONNECTED && maxTries-- > 0) {

        handle_button(); // Check if the button is pressed to enter AP mode

        delay(500);
        Serial.print(".");
        led_display_word_COn(dash); // Display "COn" on the LED strip
        dash = !dash; // Toggle dash display for visual feedback
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected.");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        // MQTT logic will go here
    } else {
        Serial.println("\nFailed to connect. Starting AP mode.");
        led_display_word_FAIL(5); // Display "FAIL" on the LED strip for 5 seconds
        startAPMode();
    }
}





/* MQTT */

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    String message = "";
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
        message += (char)payload[i];
    }
    Serial.println();

    // Handle messages based on the topic
    if (mqttTeam1Score.equals(topic)) {
        Serial.println("Handling message for team 1 score");
        team1.setScore(message.toInt());
        
    } else if (mqttTeam1ID.equals(topic)) {
        Serial.println("Handling message for team 1 ID");
        if (message.equals("A")) {
            team1.setColor(0x00FF00); // Green for Team A
        } else if (message.equals("B")) {
            team1.setColor(0xFF00FF); // Magenta for Team B
        }

    } else if (mqttTeam2Score.equals(topic)) {
        Serial.println("Handling message for team 2 score");
        team2.setScore(message.toInt());
        
    } else if (mqttTeam2ID.equals(topic)) {
        Serial.println("Handling message for team 2 ID");
        if (message.equals("A")) {
            team2.setColor(0x00FF00); // Green for Team A
        } else if (message.equals("B")) {
            team2.setColor(0xFF00FF); // Magenta for Team B
        } 

    } else {
        Serial.println("Unknown topic, no handler defined.");
    }
}

void connectToMQTT() {
    bool dash = true; // Set to true to show dash in the display

    mqttClient.setServer("f2ec5c0344964af6a9b036e32a4f726c.s1.eu.hivemq.cloud", 8883); // Replace with your MQTT server and port

    // Set MQTT username and password
    const char* mqttUser = "RCj_soccer_2025_SubOnly"; // Replace with your MQTT username
    const char* mqttPassword = "RCj_2025"; // Replace with the actual MQTT password

    // Configure secure connection using TLS without certificates
    espClient.setInsecure(); // Use this to enable TLS without requiring certificates (WiFiClientSecure)
    mqttClient.setCallback(mqttCallback);

    int maxTries = 20; // Maximum number of connection attempts
    while (!mqttClient.connected() && maxTries-- > 0) {
        led_display_word_COn(dash); // Display "COn" on the LED strip
        dash = !dash; // Toggle dash display for visual feedback

        Serial.print("Connecting to MQTT...");

        handle_button(); // Check if the button is pressed to enter AP mode
        
        String clientId = "ScoreBoardClient-" + String(random(0xffff)); // Generate a unique client ID
        if (mqttClient.connect(clientId.c_str(), mqttUser, mqttPassword)) { // Replace with a unique client ID
            Serial.println("connected.");


            // prepare topics
            mqttTeam1Score = mqttTopic + mqttFieldNum + "/team1_score";
            mqttTeam1ID = mqttTopic + mqttFieldNum + "/team1_id";
            mqttTeam2Score = mqttTopic + mqttFieldNum + "/team2_score";
            mqttTeam2ID = mqttTopic + mqttFieldNum + "/team2_id";
        
            mqttClient.subscribe(mqttTeam1Score.c_str());
            Serial.print("Subscribed to topic: ");
            Serial.println(mqttTeam1Score);

            mqttClient.subscribe(mqttTeam1ID.c_str());
            Serial.print("Subscribed to topic: ");
            Serial.println(mqttTeam1ID);


            mqttClient.subscribe(mqttTeam2Score.c_str());
            Serial.print("Subscribed to topic: ");
            Serial.println(mqttTeam2Score);

            mqttClient.subscribe(mqttTeam2ID.c_str());
            Serial.print("Subscribed to topic: ");
            Serial.println(mqttTeam2ID);

        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" retrying in 1 seconds...");
            delay(1000);
        }
    }

    if (!mqttClient.connected()) {
        Serial.println("Failed to connect to MQTT broker after multiple attempts.");
        led_display_word_FAIL(5); // Display "FAIL" on the LED strip for 5 seconds
        startAPMode(); // Start AP mode if MQTT connection fails
    } else {
        Serial.println("MQTT connected successfully.");
        led_display_ok_dashes(); // Display "OK" on the LED strip
    }

}

// Call this function after connecting to WiFi
void setupMQTT() {
    if (WiFi.status() == WL_CONNECTED) {
        connectToMQTT();
    }
}


// Add this to the main loop to maintain WiFi connection
void handleWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        connectToWiFi();
    }
}

// Add this to the main loop to maintain MQTT connection
void handleMQTT() {
    if (!mqttClient.connected()) {
        connectToMQTT();
    }
    mqttClient.loop(); 
}

