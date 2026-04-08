#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

const int LED_PIN = D9;

// TODO: Implement the WiFi event handler
// This function is called when WiFi events occur (connect, disconnect, got IP)
//
// Note: Event constant names depend on ESP32 Arduino core version:
//   Arduino-ESP32 2.x: ARDUINO_EVENT_WIFI_STA_CONNECTED, etc.
//   Arduino-ESP32 1.x: SYSTEM_EVENT_STA_CONNECTED, etc.
// PlatformIO with espressif32 platform 5.x+ uses the ARDUINO_EVENT_* names.
void onWiFiEvent(WiFiEvent_t event) {
    // Handle these events:
    // - ARDUINO_EVENT_WIFI_STA_CONNECTED: Connected to access point
    // - ARDUINO_EVENT_WIFI_STA_GOT_IP: Got IP address
    // - ARDUINO_EVENT_WIFI_STA_DISCONNECTED: Disconnected from access point
    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("WiFi connected to access point");
            break;

        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.print("Got IP address: ");
            Serial.println(WiFi.localIP());
            digitalWrite(LED_PIN, HIGH); // on = connected
            break;

        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("WiFi disconnected");
            digitalWrite(LED_PIN, LOW);
            break;

        default:
            break;
    }
}

// TODO: Implement the connect function
// Should initiate connection and wait until connected or timeout
bool connectToWiFi() {
    // 1. Set WiFi mode to station (WIFI_STA)
    // 2. Begin connection with WiFi.begin(ssid, password)
    // 3. Wait for connection with timeout (e.g., 10 seconds)
    // 4. Return true if connected, false if timeout
    Serial.print("Connecting to ");
    Serial.print(WIFI_SSID);
    Serial.println("...");

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long startTime = millis();
    const unsigned long timeout = 10000;
    bool ledState = false;

    while (WiFi.status() != WL_CONNECTED && millis() - startTime < timeout) {
        ledState = !ledState;          // blinking = connecting
        digitalWrite(LED_PIN, ledState);

        delay(500);
        Serial.print(".");
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        digitalWrite(LED_PIN, HIGH);
        return true;
    }

    digitalWrite(LED_PIN, LOW);
    return false;
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("ESP32 WiFi Connect Example");
    Serial.println("============================");

    // TODO: Register the WiFi event handler
    // WiFi.onEvent(onWiFiEvent);
    WiFi.onEvent(onWiFiEvent);

    // TODO: Call connectToWiFi() and handle result
    bool connected = connectToWiFi();

    if (connected) {
        Serial.println("Connection successful!");
    } else {
        Serial.println("Connection failed!");
    }
}

void loop() {
    // TODO: Check connection status periodically
    // If disconnected, attempt to reconnect
    if (WiFi.status() != WL_CONNECTED) {
        static bool ledState = false;

        Serial.println("Reconnecting...");
        WiFi.reconnect();

        ledState = !ledState;          // blinking while reconnecting
        digitalWrite(LED_PIN, ledState);

        delay(500);
    } else {
        digitalWrite(LED_PIN, HIGH);   // on = connected
        delay(5000);
    }
}