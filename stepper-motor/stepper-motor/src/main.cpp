#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Stepper motor pins (ULN2003)
const int MOTOR_PIN1 = 14;
const int MOTOR_PIN2 = 27;
const int MOTOR_PIN3 = 26;
const int MOTOR_PIN4 = 25;

// Stepper configuration
const int STEPS_PER_REV = 2048;  // 28BYJ-48 with gear reduction
const int STEP_DELAY = 3;        // Milliseconds between steps

// 4-step sequence for 28BYJ-48
const int stepSequence[4][4] = {
    {1, 0, 0, 1},
    {1, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 1}
};

int currentStep = 0;

// BME280 sensor
Adafruit_BME280 bme;

void setMotorStep(int step) {
    // TODO: Set motor pins based on step sequence
    // digitalWrite(MOTOR_PIN1, stepSequence[step][0]);
    // digitalWrite(MOTOR_PIN2, stepSequence[step][1]);
    // digitalWrite(MOTOR_PIN3, stepSequence[step][2]);
    // digitalWrite(MOTOR_PIN4, stepSequence[step][3]);

    digitalWrite(MOTOR_PIN1, stepSequence[step][0]);
    digitalWrite(MOTOR_PIN2, stepSequence[step][1]);
    digitalWrite(MOTOR_PIN3, stepSequence[step][2]);
    digitalWrite(MOTOR_PIN4, stepSequence[step][3]);
}

void stepMotor(int steps) {
    // TODO: Move motor by specified number of steps
    // Positive = clockwise, negative = counter-clockwise
    // for (int i = 0; i < abs(steps); i++) {
    //     if (steps > 0) {
    //         currentStep = (currentStep + 1) % 4;
    //     } else {
    //         currentStep = (currentStep + 3) % 4;  // -1 mod 4
    //     }
    //     setMotorStep(currentStep);
    //     delay(STEP_DELAY);
    // }

    for (int i = 0; i < abs(steps); i++) {
        if (steps > 0) {
            currentStep = (currentStep + 1) % 4;
        } else {
            currentStep = (currentStep + 3) % 4;  // -1 mod 4
        }

        setMotorStep(currentStep);
        delay(STEP_DELAY);
    }
}

void motorOff() {
    // Turn off all motor coils to save power and reduce heat
    digitalWrite(MOTOR_PIN1, LOW);
    digitalWrite(MOTOR_PIN2, LOW);
    digitalWrite(MOTOR_PIN3, LOW);
    digitalWrite(MOTOR_PIN4, LOW);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Temperature-Controlled Window Simulation");

    // TODO: Configure motor pins as outputs
    // pinMode(MOTOR_PIN1, OUTPUT);
    // pinMode(MOTOR_PIN2, OUTPUT);
    // pinMode(MOTOR_PIN3, OUTPUT);
    // pinMode(MOTOR_PIN4, OUTPUT);

    pinMode(MOTOR_PIN1, OUTPUT);
    pinMode(MOTOR_PIN2, OUTPUT);
    pinMode(MOTOR_PIN3, OUTPUT);
    pinMode(MOTOR_PIN4, OUTPUT);

    motorOff();

    // TODO: Initialize I2C and BME280
    // Wire.begin(21, 22);
    // if (!bme.begin(0x76)) {
    //     Serial.println("BME280 not found!");
    //     while (1);
    // }

    Wire.begin(21, 22);

    if (!bme.begin(0x76)) {
        if (!bme.begin(0x77)) {
            Serial.println("BME280 not found!");
            while (1) {
                delay(100);
            }
        }
    }

    Serial.println("System ready");
    Serial.println("Temperature range: 20-30°C");
    Serial.println("Window: Closed < 23°C, Open > 27°C");
}

void loop() {
    // TODO: Read temperature from BME280
    // float temperature = bme.readTemperature();

    float temperature = bme.readTemperature();

    // TODO: Map temperature to window position
    // Temperature range: 20-30°C
    // Window range: 0 (closed) to 512 steps (fully open)
    // Example:
    // int targetPosition = map(temperature * 10, 200, 300, 0, 512);
    // targetPosition = constrain(targetPosition, 0, 512);

    int targetPosition = 0;
    const char* windowState = "closed";

    if (temperature < 23.0) {
        targetPosition = 0;
        windowState = "closed";
    } else if (temperature > 27.0) {
        targetPosition = 512;
        windowState = "fully open";
    } else {
        targetPosition = map((int)(temperature * 10), 230, 270, 0, 512);
        targetPosition = constrain(targetPosition, 0, 512);

        if (targetPosition >= 246 && targetPosition <= 266) {
            windowState = "half open";
        } else {
            windowState = "partially open";
        }
    }

    // TODO: Calculate steps needed to reach target
    // Track current position and move difference
    // static int currentPosition = 0;
    // int stepsToMove = targetPosition - currentPosition;

    static int currentPosition = 0;
    int stepsToMove = targetPosition - currentPosition;

    // TODO: Move motor to target position
    // if (stepsToMove != 0) {
    //     Serial.print("Temp: ");
    //     Serial.print(temperature);
    //     Serial.print("°C -> Moving window ");
    //     Serial.print(stepsToMove);
    //     Serial.println(" steps");
    //     stepMotor(stepsToMove);
    //     currentPosition = targetPosition;
    //     motorOff();  // Save power when not moving
    // }

    Serial.print("Temp: ");
    Serial.print(temperature, 1);
    Serial.print("°C -> Moving window ");
    Serial.print(targetPosition);
    Serial.print(" steps (");
    Serial.print(windowState);
    Serial.println(")");

    if (stepsToMove != 0) {
        stepMotor(stepsToMove);
        currentPosition = targetPosition;
        motorOff();  // Save power when not moving
    }

    delay(5000);  // Check temperature every 5 seconds
}