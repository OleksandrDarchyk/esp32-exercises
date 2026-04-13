#include <Arduino.h>
#include <Wire.h>

// Default I2C pins for ESP32
// SDA = data line, SCL = clock line
// For FireBeetle 2 ESP32-E these pins are commonly:
const int SDA_PIN = 21;
const int SCL_PIN = 22;

// Forward declaration.
// This tells the compiler that the function exists below.
// Without this, scanI2C() may not know about identifyDevice().
const char* identifyDevice(int address);

// TODO: Implement function to scan I2C bus
void scanI2C() {
    // 1. Print header
    // We print a title every time scanning starts,
    // so it is easy to see a new scan in Serial Monitor.
    Serial.println("Scanning I2C bus...");
    Serial.println();

    int devicesFound = 0;

    // 2. Loop through all possible 7-bit addresses (0x01 to 0x7F)
    //    Address 0x00 is reserved for general call
    //
    // I2C usually uses 7-bit addresses.
    // That gives addresses from 0x00 to 0x7F.
    //
    // We do NOT scan 0x00 because it is reserved.
    // So we scan from 1 to 126.
    //
    // Important:
    // address < 127 means the last checked address is 126.
    for (int address = 1; address < 127; address++) {
        // 3. Begin transmission to address
        //    Wire.beginTransmission(address);
        //
        // This prepares an I2C transmission to one specific address.
        // Think of it like:
        // "Hello, device at this address, are you there?"
        Wire.beginTransmission(address);

        // 4. End transmission and check result
        //    byte error = Wire.endTransmission();
        //    error == 0 means device acknowledged
        //
        // endTransmission() finishes the address check.
        //
        // Most important return values here:
        // 0 = success, device answered with ACK
        // 4 = other/unknown bus error
        //
        // If it is not 0, usually nothing answered on that address.
        byte error = Wire.endTransmission();

        // 5. If device found:
        //    - Print address in hex format (0x00)
        //    - Increment device counter
        //    - Identify known devices (see table below)
        if (error == 0) {
            // Print the address in hexadecimal format.
            // %02X means:
            // %X  -> print number as HEX
            // 02  -> minimum width 2 characters, add leading zero if needed
            //
            // Example:
            // 10 decimal -> 0A hex
            // 118 decimal -> 76 hex
            Serial.printf("Found device at 0x%02X - %s\n", address, identifyDevice(address));

            // Count how many devices answered.
            devicesFound++;
        }
        else if (error == 4) {
            // Optional: print unknown/bus errors.
            // This is useful for debugging wiring or bus problems.
            Serial.printf("Unknown error at 0x%02X\n", address);
        }

        // If error is something else, we do nothing.
        // That simply means there is probably no device on that address.
    }

    // 6. Print summary
    Serial.println();
    if (devicesFound == 0) {
        Serial.println("No I2C devices found!");
    } else {
        Serial.printf("Found %d device(s)\n", devicesFound);
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