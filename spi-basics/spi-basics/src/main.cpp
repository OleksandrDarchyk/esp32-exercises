#include <Arduino.h>
#include <SPI.h>

// Default VSPI pins on ESP32
#define VSPI_MOSI 23
#define VSPI_MISO 19
#define VSPI_SCLK 18
#define VSPI_CS   5

// Global variable to store last received value
byte lastReceivedValue = 0;

// Loopback test function
bool testLoopback(byte testValue) {
    // 1. Pull CS low (optional for loopback, but good practice)
    digitalWrite(VSPI_CS, LOW);

    // 2. Transfer a byte and capture the response
    byte received = SPI.transfer(testValue);
    lastReceivedValue = received;

    // 3. Pull CS high
    digitalWrite(VSPI_CS, HIGH);

    // 4. Compare sent and received values
    return (testValue == received);
}

// Function to test multiple values
void runLoopbackTests() {
    Serial.println("Running SPI loopback tests...");
    Serial.println();

    // Test values to send
    byte testValues[] = {0x00, 0xFF, 0xAA, 0x55, 0x42};
    int numTests = sizeof(testValues) / sizeof(testValues[0]);
    int passed = 0;

    for (int i = 0; i < numTests; i++) {
        byte value = testValues[i];

        bool result = testLoopback(value);

        Serial.printf("Sent: 0x%02X, Received: 0x%02X - %s\n",
                      value,
                      lastReceivedValue,
                      result ? "PASS" : "FAIL");

        if (result) {
            passed++;
        }
    }

    Serial.println();
    Serial.printf("Results: %d/%d tests passed\n", passed, numTests);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("SPI Loopback Test");
    Serial.println("=================");
    Serial.println();
    Serial.println("Ensure MOSI (GPIO 23) is connected to MISO (GPIO 19)");
    Serial.println();

    // Configure CS pin as output
    pinMode(VSPI_CS, OUTPUT);
    digitalWrite(VSPI_CS, HIGH);  // Inactive

    // Initialize SPI
    SPI.begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, VSPI_CS);

    // Configure SPI settings
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
}

void loop() {
    // Run loopback tests
    runLoopbackTests();

    Serial.println("\nPress reset to run again...");
    while (true) {
        delay(1000);
    }
}