#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LiquidCrystal_I2C.h>

// BME280 I2C address (0x76 or 0x77)
// Check with I2C scanner if unsure
#define BME280_ADDRESS 0x76

// LCD I2C address (usually 0x27 or 0x3F)
#define LCD_ADDRESS 0x27

// Sea level pressure for altitude calculation (hPa)
#define SEALEVELPRESSURE_HPA 1013.25

Adafruit_BME280 bme;
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

// TODO: Implement sensor initialization
bool initSensor() {
    // 1. Initialize I2C with Wire.begin()
    Wire.begin(21, 22);

    // 2. Initialize BME280 with bme.begin(address)
    //    Returns false if sensor not found
    bool found = bme.begin(BME280_ADDRESS);
    if (!found) {
        return false;
    }

    // 3. Configure sensor settings (optional):
    //    bme.setSampling(mode, tempSampling, pressSampling,
    //                    humSampling, filter, duration);
    bme.setSampling(
        Adafruit_BME280::MODE_NORMAL,
        Adafruit_BME280::SAMPLING_X2,    // temperature
        Adafruit_BME280::SAMPLING_X16,   // pressure
        Adafruit_BME280::SAMPLING_X1,    // humidity
        Adafruit_BME280::FILTER_X16,
        Adafruit_BME280::STANDBY_MS_500
    );

    return true;
}

// TODO: Implement function to read and display sensor data
void readSensor() {
    // 1. Read temperature with bme.readTemperature()
    //    Returns temperature in Celsius
    float temperature = bme.readTemperature();

    // 2. Read humidity with bme.readHumidity()
    //    Returns relative humidity in %
    float humidity = bme.readHumidity();

    // 3. Read pressure with bme.readPressure()
    //    Returns pressure in Pascals (divide by 100 for hPa)
    float pressure = bme.readPressure() / 100.0F;

    // 4. Calculate altitude with bme.readAltitude(SEALEVELPRESSURE_HPA)
    //    Returns approximate altitude in meters
    float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

    // 5. Print formatted output
    Serial.printf("Temperature: %.2f C\n", temperature);
    Serial.printf("Humidity:    %.1f %%\n", humidity);
    Serial.printf("Pressure:    %.2f hPa\n", pressure);
    Serial.printf("Altitude:    %.1f m\n", altitude);
    Serial.println();

    // Перший рядок LCD: температура
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.print(temperature, 1);
    lcd.print((char)223); // degree symbol
    lcd.print("C   ");

    // Другий рядок LCD: вологість
    lcd.setCursor(0, 1);
    lcd.print("Hum: ");
    lcd.print(humidity, 1);
    lcd.print("%   ");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("BME280 Sensor Example");
    Serial.println("=====================");

    // Ініціалізація LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Starting...");
    lcd.setCursor(0, 1);
    lcd.print("Please wait");

    // TODO: Initialize sensor
    // If initialization fails, print error and halt
    if (!initSensor()) {
        Serial.println("ERROR: BME280 sensor not found!");
        Serial.println("Check the following:");
        Serial.println("- Wiring: VCC, GND, SDA, SCL");
        Serial.println("- I2C address: 0x76 or 0x77");
        Serial.println("- Sensor is connected correctly");

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("BME280 error");
        lcd.setCursor(0, 1);
        lcd.print("Check sensor");

        while (true) {
            delay(1000);
        }
    }

    Serial.println("Sensor initialized successfully!");
    Serial.println();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor OK");
    delay(1000);
    lcd.clear();
}

void loop() {
    // TODO: Read and display sensor data
    readSensor();

    delay(2000);
}