#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"

const int LDR_PIN = 34;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

unsigned long lastPublish = 0;
const unsigned long PUBLISH_INTERVAL = 5000;

// Connect to WiFi (you can reuse code from Exercise WiFi Connect)
void connectWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

// TODO: Implement MQTT connection function
// Should connect to broker and return true/false
bool connectMQTT() {
    // 1. Check if already connected with mqttClient.connected()
    if (mqttClient.connected()) {
        return true;
    }

    // 2. Generate a unique client ID (e.g., "esp32-" + random number)
    String clientId = "esp32-" + String(random(0xffff), HEX);

    // 3. Attempt connection with mqttClient.connect(clientId, MQTT_TOKEN, "")
    //    Note: For Flespi, username is the token, password is empty
    if (mqttClient.connect(clientId.c_str(), MQTT_TOKEN, "")) {
        // 4. Print success or failure message
        Serial.println("MQTT connected!");
        // 5. Return connection status
        return true;
    } else {
        Serial.print("MQTT connection failed, rc=");
        Serial.println(mqttClient.state());
        return false;
    }
}

// TODO: Implement function to read sensor and publish
void publishSensorData() {
    // 1. Read the LDR value using analogRead()
    int ldrValue = analogRead(LDR_PIN);

    // 2. Convert to string using String() or snprintf()
    char payload[16];
    snprintf(payload, sizeof(payload), "%d", ldrValue);

    // 3. Publish using mqttClient.publish(topic, payload)
    mqttClient.publish(MQTT_TOPIC, payload);

    // 4. Print confirmation to Serial
    Serial.print("Published to ");
    Serial.print(MQTT_TOPIC);
    Serial.print(": ");
    Serial.println(payload);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("MQTT Publish Example");
    Serial.println("====================");

    pinMode(LDR_PIN, INPUT);

    connectWiFi();

    // TODO: Configure MQTT client
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);

    // TODO: Connect to MQTT broker
    connectMQTT();
}

void loop() {
    // TODO: Ensure MQTT connection is maintained
    // mqttClient.loop() must be called regularly
    if (!mqttClient.connected()) {
        connectMQTT();
    }
    mqttClient.loop();

    // TODO: Publish sensor data every 5 seconds
    // Use millis() for non-blocking timing
    if (millis() - lastPublish >= PUBLISH_INTERVAL) {
        lastPublish = millis();
        publishSensorData();
    }

    delay(100);
}