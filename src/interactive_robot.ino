/*
 * ESP32 Interactive Robot — reference implementation
 *
 * Assumed hardware: classic ESP32 DevKit, TB6612FNG dual motor driver,
 * two brushed DC motors, HC-SR04 ultrasonic sensor, and a start/stop button.
 * These parts and pin assignments are a generated reference build. Confirm
 * they match your physical robot before connecting power.
 */

#include <Arduino.h>

constexpr uint8_t PIN_AIN1 = 26;
constexpr uint8_t PIN_AIN2 = 27;
constexpr uint8_t PIN_PWMA = 25;
constexpr uint8_t PIN_BIN1 = 33;
constexpr uint8_t PIN_BIN2 = 32;
constexpr uint8_t PIN_PWMB = 14;
constexpr uint8_t PIN_STBY = 13;

constexpr uint8_t PIN_TRIG = 18;
constexpr uint8_t PIN_ECHO = 19;  // HC-SR04 ECHO must pass through a divider.
constexpr uint8_t PIN_START_BUTTON = 23; // Button from GPIO23 to GND.

constexpr uint32_t ECHO_TIMEOUT_US = 25000UL;
constexpr int OBSTACLE_DISTANCE_CM = 25;
constexpr uint32_t DEBOUNCE_MS = 35;

bool robotEnabled = false;
bool lastButtonReading = HIGH;
bool stableButtonState = HIGH;
uint32_t lastDebounceMs = 0;

void setWheel(uint8_t in1, uint8_t in2, uint8_t pwm, int direction) {
  if (direction > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(pwm, HIGH); // Full speed reference build; no PWM speed control.
  } else if (direction < 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(pwm, HIGH);
  } else {
    digitalWrite(pwm, LOW);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
}

void drive(int leftDirection, int rightDirection) {
  setWheel(PIN_AIN1, PIN_AIN2, PIN_PWMA, leftDirection);
  setWheel(PIN_BIN1, PIN_BIN2, PIN_PWMB, rightDirection);
}

void stopMotors() {
  drive(0, 0);
}

long readDistanceCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(3);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  const unsigned long echoUs = pulseIn(PIN_ECHO, HIGH, ECHO_TIMEOUT_US);
  if (echoUs == 0) {
    return -1; // No echo: stop instead of assuming the path is clear.
  }
  return static_cast<long>(echoUs / 58UL);
}

void pollStartButton() {
  const bool reading = digitalRead(PIN_START_BUTTON);

  if (reading != lastButtonReading) {
    lastDebounceMs = millis();
  }

  if ((millis() - lastDebounceMs) > DEBOUNCE_MS &&
      reading != stableButtonState) {
    stableButtonState = reading;
    if (stableButtonState == LOW) {
      robotEnabled = !robotEnabled;
      stopMotors();
      Serial.println(robotEnabled ? "Robot: RUN" : "Robot: STOP");
    }
  }

  lastButtonReading = reading;
}

void avoidObstacle() {
  drive(-1, -1);
  delay(250);
  stopMotors();
  delay(100);

  // Pivot right. Swap signs if the motor wiring makes the robot turn left.
  drive(1, -1);
  delay(350);
  stopMotors();
  delay(100);
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_AIN1, OUTPUT);
  pinMode(PIN_AIN2, OUTPUT);
  pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_BIN1, OUTPUT);
  pinMode(PIN_BIN2, OUTPUT);
  pinMode(PIN_PWMB, OUTPUT);
  pinMode(PIN_STBY, OUTPUT);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_START_BUTTON, INPUT_PULLUP);

  digitalWrite(PIN_STBY, HIGH);
  digitalWrite(PIN_TRIG, LOW);
  stopMotors();

  Serial.println("ESP32 Interactive Robot reference firmware");
  Serial.println("Press the button to start or stop.");
}

void loop() {
  pollStartButton();

  if (!robotEnabled) {
    stopMotors();
    delay(10);
    return;
  }

  const long distanceCm = readDistanceCm();
  if (distanceCm < 0) {
    stopMotors();
    Serial.println("No ultrasonic echo; motors stopped.");
  } else if (distanceCm <= OBSTACLE_DISTANCE_CM) {
    Serial.printf("Obstacle at %ld cm; avoiding.\n", distanceCm);
    avoidObstacle();
  } else {
    drive(1, 1);
  }

  delay(60); // Leave time between HC-SR04 measurements.
}
