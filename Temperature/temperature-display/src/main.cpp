#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// DS18B20 Configuration
// D12 on your FireBeetle board corresponds to GPIO 4.
// So ONE_WIRE_PIN = 4 is correct if the sensor DATA wire is on D12.
#define ONE_WIRE_PIN 4
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

// LCD Configuration
#define LCD_ADDRESS 0x27  // Use I2C Scanner to find correct address
#define LCD_COLUMNS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("DS18B20 Temperature Display");

    // Initialize I2C
    // LCD uses I2C on ESP32:
    // SDA = GPIO 21
    // SCL = GPIO 22
    Wire.begin(21, 22);

    // TODO: Initialize LCD
    // lcd.init();
    // lcd.backlight();
    // lcd.setCursor(0, 0);
    // lcd.print("Temp Monitor");
    //
    // Start LCD, turn on backlight, and print title on first row.
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Temp Monitor");

    // TODO: Initialize DS18B20
    // sensors.begin();
    // Print number of devices found:
    // Serial.print("Found ");
    // Serial.print(sensors.getDeviceCount());
    // Serial.println(" devices");
    //
    // Start the DS18B20 library and check how many sensors are found.
    sensors.begin();

    Serial.print("Found ");
    Serial.print(sensors.getDeviceCount());
    Serial.println(" device(s)");

    // If no sensor is found, show message on LCD.
    if (sensors.getDeviceCount() == 0) {
        lcd.setCursor(0, 1);
        lcd.print("No sensor found");
    } else {
        lcd.setCursor(0, 1);
        lcd.print("Starting...    ");
    }

    delay(1500);
}

void loop() {
    // TODO: Request temperature reading
    // sensors.requestTemperatures();
    //
    // Tell DS18B20 to perform a temperature measurement now.
    sensors.requestTemperatures();

    // TODO: Read temperature in Celsius
    // float tempC = sensors.getTempCByIndex(0);
    //
    // Read temperature from the first sensor found on the bus.
    float tempC = sensors.getTempCByIndex(0);

    // DS18B20 returns DEVICE_DISCONNECTED_C when the sensor
    // is not connected or cannot be read correctly.
    if (tempC == DEVICE_DISCONNECTED_C) {
        lcd.setCursor(0, 0);
        lcd.print("Temp Monitor    ");
        lcd.setCursor(0, 1);
        lcd.print("Sensor error    ");

        Serial.println("Temperature: sensor error");
        delay(2000);
        return;
    }

    // TODO: Display on LCD
    // lcd.setCursor(0, 1);  // Second row
    // lcd.print("Temp: ");
    // lcd.print(tempC, 1);  // 1 decimal place
    // lcd.print(" C  ");    // Extra spaces to clear old digits
    //
    // Print temperature on second row.
    // Extra spaces clear leftover characters from older values.
    lcd.setCursor(0, 0);
    lcd.print("Temp Monitor    ");

    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print(tempC, 1);
    lcd.print(" C   ");

    // TODO: Also print to Serial Monitor
    // Serial.print("Temperature: ");
    // Serial.print(tempC);
    // Serial.println(" C");
    Serial.print("Temperature: ");
    Serial.print(tempC, 1);
    Serial.println(" C");

    delay(2000);  // Update every 2 seconds
}