#ifndef CONFIG_H
#define CONFIG_H

// WiFi credentials
const char* WIFI_SSID = "POCO F3";
const char* WIFI_PASSWORD = "eaxb0711";

// Flespi MQTT broker settings
const char* MQTT_BROKER = "mqtt.flespi.io";
const int MQTT_PORT = 1883;
const char* MQTT_TOKEN = "0x6p8lOm4k2S854Q79KcQlN07ehH7nqmjQrTNknEewCtjmMVyUOjH9jFtYXKtMPA";

// TODO: Create a unique topic name using your initials or student ID
// Example: "iot-course/jd123/sensor"
const char* MQTT_TOPIC = "iot-course/Oleksandr/sensor";

#endif