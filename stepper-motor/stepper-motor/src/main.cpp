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

// Window position range
const int WINDOW_MIN_STEPS = 0;
const int WINDOW_MAX_STEPS = 512;
const int WINDOW_HALF_STEPS = 256;

// 4-step sequence for 28BYJ-48
const int stepSequence[4][4] = {
    {1, 0, 0, 1},
    {1, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 1}
};

int currentStep = 0;

// Window state
int currentPosition = 0;
bool manualMode = false;
int manualTargetPosition = 0;

// BME280 sensor
Adafruit_BME280 bme;

// Timer for temperature update
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 5000;

void setMotorStep(int step) {
    digitalWrite(MOTOR_PIN1, stepSequence[step][0]);
    digitalWrite(MOTOR_PIN2, stepSequence[step][1]);
    digitalWrite(MOTOR_PIN3, stepSequence[step][2]);
    digitalWrite(MOTOR_PIN4, stepSequence[step][3]);
}

void stepMotor(int steps) {
    for (int i = 0; i < abs(steps); i++) {
        if (steps > 0) {
            currentStep = (currentStep + 1) % 4;
        } else {
            currentStep = (currentStep + 3) % 4;
        }

        setMotorStep(currentStep);
        delay(STEP_DELAY);
    }
}

void motorOff() {
    digitalWrite(MOTOR_PIN1, LOW);
    digitalWrite(MOTOR_PIN2, LOW);
    digitalWrite(MOTOR_PIN3, LOW);
    digitalWrite(MOTOR_PIN4, LOW);
}

void moveWindowTo(int targetPosition) {
    targetPosition = constrain(targetPosition, WINDOW_MIN_STEPS, WINDOW_MAX_STEPS);

    int stepsToMove = targetPosition - currentPosition;

    if (stepsToMove != 0) {
        stepMotor(stepsToMove);
        currentPosition = targetPosition;
        motorOff();
    }
}

void printStatus(float temperature, int targetPosition, const char* windowState) {
    Serial.print("Temp: ");
    Serial.print(temperature, 2);
    Serial.print("°C -> Window target: ");
    Serial.print(targetPosition);
    Serial.print(" steps (");
    Serial.print(windowState);
    Serial.print(") | Current: ");
    Serial.print(currentPosition);
    Serial.print(" | Mode: ");
    Serial.println(manualMode ? "MANUAL" : "AUTO");
}

void handleSerialCommands() {
    if (!Serial.available()) {
        return;
    }

    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();

    Serial.print("Received: [");
    Serial.print(command);
    Serial.println("]");

    if (command == "open") {
        manualMode = true;
        manualTargetPosition = WINDOW_MAX_STEPS;
        Serial.println("Manual command: OPEN");
        moveWindowTo(manualTargetPosition);
    } else if (command == "close") {
        manualMode = true;
        manualTargetPosition = WINDOW_MIN_STEPS;
        Serial.println("Manual command: CLOSE");
        moveWindowTo(manualTargetPosition);
    } else if (command == "half") {
        manualMode = true;
        manualTargetPosition = WINDOW_HALF_STEPS;
        Serial.println("Manual command: HALF");
        moveWindowTo(manualTargetPosition);
    } else if (command == "auto") {
        manualMode = false;
        Serial.println("Switched to AUTO mode");
    } else if (command == "status") {
        Serial.print("Current position: ");
        Serial.print(currentPosition);
        Serial.print(" | Mode: ");
        Serial.println(manualMode ? "MANUAL" : "AUTO");
    } else if (command.length() > 0) {
        Serial.println("Unknown command. Use: open, close, half, auto, status");
    }
}

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(50);

    Serial.println("Temperature-Controlled Window Simulation");

    pinMode(MOTOR_PIN1, OUTPUT);
    pinMode(MOTOR_PIN2, OUTPUT);
    pinMode(MOTOR_PIN3, OUTPUT);
    pinMode(MOTOR_PIN4, OUTPUT);

    motorOff();

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
    Serial.println("Serial commands: open, close, half, auto, status");
}

void loop() {
    handleSerialCommands();

    if (millis() - lastUpdate >= UPDATE_INTERVAL) {
        lastUpdate = millis();

        float temperature = bme.readTemperature();

        int targetPosition = 0;
        const char* windowState = "closed";

        if (manualMode) {
            targetPosition = manualTargetPosition;

            if (targetPosition == WINDOW_MIN_STEPS) {
                windowState = "closed";
            } else if (targetPosition == WINDOW_HALF_STEPS) {
                windowState = "half open";
            } else if (targetPosition == WINDOW_MAX_STEPS) {
                windowState = "fully open";
            } else {
                windowState = "manual position";
            }
        } else {
            if (temperature < 23.0) {
                targetPosition = WINDOW_MIN_STEPS;
                windowState = "closed";
            } else if (temperature > 27.0) {
                targetPosition = WINDOW_MAX_STEPS;
                windowState = "fully open";
            } else {
                targetPosition = map((int)(temperature * 10), 230, 270, WINDOW_MIN_STEPS, WINDOW_MAX_STEPS);
                targetPosition = constrain(targetPosition, WINDOW_MIN_STEPS, WINDOW_MAX_STEPS);

                if (targetPosition >= 246 && targetPosition <= 266) {
                    windowState = "half open";
                } else {
                    windowState = "partially open";
                }
            }

            moveWindowTo(targetPosition);
        }

        printStatus(temperature, targetPosition, windowState);
    }
}