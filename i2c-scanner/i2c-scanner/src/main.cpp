#include <Arduino.h>
#include <Wire.h>

// Default I2C pins for ESP32
// SDA = data line, SCL = clock line
// For FireBeetle 2 ESP32-E these pins are commonly:
const int SDA_PIN = 21;
const int SCL_PIN = 22;

const int LED_PIN = 25; // D2 on your board

// Forward declaration.
// This tells the compiler that the function exists below.
// Without this, scanI2C() may not know about identifyDevice().
const char* identifyDevice(int address);

// TODO: Implement function to scan I2C bus
void scanI2C() {
    Serial.println("Scanning I2C bus...");
    Serial.println();

    int devicesFound = 0;

    // Print top header
    Serial.println("    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");

    // Rows: 0x00, 0x10, 0x20 ... 0x70
    for (int row = 0; row < 8; row++) {
        int baseAddress = row * 16;

        // Print row label: 00:, 10:, 20: ...
        Serial.printf("%02X: ", baseAddress);

        // Columns: 0 ... F
        for (int col = 0; col < 16; col++) {
            int address = baseAddress + col;

            // 0x00 is reserved, so we usually skip it
            if (address == 0x00) {
                Serial.print("   ");
                continue;
            }

            Wire.beginTransmission(address);
            byte error = Wire.endTransmission();

            if (error == 0) {
                // Device found -> print address
                Serial.printf("%02X ", address);
                devicesFound++;
            } else {
                // No device -> print --
                Serial.print("-- ");
            }
        }

        Serial.println();
    }

    Serial.println();

    if (devicesFound == 0) {
        Serial.println("No I2C devices found!");
        digitalWrite(LED_PIN, LOW);
    } else {
        Serial.printf("Found %d device(s)\n", devicesFound);
        digitalWrite(LED_PIN, HIGH);
    }
}
// TODO: Implement function to identify common I2C devices
const char* identifyDevice(int address) {
    // Return device name based on address
    // Common addresses:
    // 0x27 or 0x3F - LCD with I2C backpack
    // 0x3C or 0x3D - OLED display (SSD1306)
    // 0x40 - HDC1080 temperature/humidity
    // 0x48-0x4F - ADS1115 ADC
    // 0x50-0x57 - EEPROM
    // 0x68 - DS3231 RTC, MPU6050
    // 0x76 or 0x77 - BME280, BMP280
    //
    // Important:
    // This function does NOT truly detect the exact sensor model.
    // It only gives a likely device name based on a common address.
    //
    // Example:
    // 0x76 could be BME280 or BMP280.
    // So we print both names as a hint.

    switch (address) {
        case 0x27:
        case 0x3F:
            return "LCD with I2C backpack";

        case 0x3C:
        case 0x3D:
            return "OLED Display (SSD1306)";

        case 0x40:
            return "HDC1080 temp/humidity";

        case 0x68:
            return "DS3231 RTC or MPU6050";

        case 0x76:
        case 0x77:
            return "BME280/BMP280";

        default:
            // Some devices use a whole range of possible addresses.
            if (address >= 0x48 && address <= 0x4F) {
                return "ADS1115 ADC";
            }

            if (address >= 0x50 && address <= 0x57) {
                return "EEPROM";
            }

            return "Unknown";
    }
}

void setup() {
    // Start serial communication.
    // 115200 must match the value in platformio.ini monitor_speed.
    Serial.begin(115200);

    // Small delay so the Serial Monitor has time to start properly.
    delay(1000);

    Serial.println("I2C Scanner");
    Serial.println("============");

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // TODO: Initialize I2C with Wire.begin(SDA, SCL)
    //
    // This starts the I2C bus using the chosen ESP32 pins.
    // SDA = GPIO 21
    // SCL = GPIO 22
    //
    // After this, the ESP32 can communicate with I2C devices.
    Wire.begin(SDA_PIN, SCL_PIN);

    // Optional info so you clearly see which pins are used.
    Serial.printf("Using SDA = GPIO %d\n", SDA_PIN);
    Serial.printf("Using SCL = GPIO %d\n", SCL_PIN);
    Serial.printf("Using LED = GPIO %d\n", LED_PIN);
    Serial.println();

    // TODO: Call scan function
    //
    // Run one scan immediately when the board starts.
    scanI2C();
}

void loop() {
    // TODO: Scan every 5 seconds (optional)
    // This helps when hot-plugging devices
    //
    // "Hot-plugging" means connecting a module while the ESP32 is already running.
    //
    // Every 5 seconds we scan again so newly connected devices can appear.
    Serial.println();
    Serial.println("------------------------------");
    scanI2C();

    delay(5000);
}