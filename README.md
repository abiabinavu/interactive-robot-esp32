# ESP32 Interactive Robot

This repository contains a **generated reference implementation** based on the project summary in my resume. It demonstrates an ESP32 robot that can be started/stopped by a button and can avoid nearby obstacles with an ultrasonic sensor.

> The exact sensor, driver, and wiring below are assumed choices because the original project files were not supplied. This code and diagram have not been verified against the physical robot. Compare them with your actual parts before powering the circuit.

## Project features

- ESP32 firmware written for the Arduino framework
- Start/stop control with a push button
- HC-SR04 distance reading
- Two DC motors driven through a TB6612FNG dual motor driver
- Stops when the ultrasonic sensor has no echo
- Reverses and pivots when an obstacle is within the configured distance

## Assumed components

- Classic ESP32 DevKit board
- TB6612FNG motor-driver breakout
- Two brushed DC gear motors and a motor battery matched to the motors
- HC-SR04 ultrasonic module
- Momentary push button
- 2 kΩ and 3.3 kΩ resistors for the HC-SR04 ECHO divider
- Jumper wires and a chassis

Choose motors whose stall current is within the motor-driver breakout’s limits. Toshiba lists 1.2 A average and 3.2 A peak output current for the TB6612FNG IC; confirm the limits of your specific breakout and motor supply before use.

## Circuit diagram

![ESP32 interactive robot reference wiring diagram](docs/circuit.svg)

## Wiring

| Module / signal | Connect to |
| --- | --- |
| TB6612FNG VCC | ESP32 3V3 |
| TB6612FNG VM | Separate motor battery positive, within motor and driver board ratings |
| TB6612FNG GND | ESP32 GND and motor-battery negative (common ground) |
| AIN1 / AIN2 / PWMA | ESP32 GPIO 26 / 27 / 25 |
| BIN1 / BIN2 / PWMB | ESP32 GPIO 33 / 32 / 14 |
| STBY | ESP32 GPIO 13 |
| A01 / A02 | Left motor terminals |
| B01 / B02 | Right motor terminals |
| HC-SR04 VCC / GND | 5V / common GND |
| HC-SR04 TRIG | ESP32 GPIO 18 |
| HC-SR04 ECHO | Through divider below to ESP32 GPIO 19 |
| Start/stop button | Between ESP32 GPIO 23 and GND |

### HC-SR04 ECHO divider

The HC-SR04 is powered from 5V. To keep its ECHO signal within the ESP32 input range, wire **ECHO → 2 kΩ → GPIO19 node**, then connect **GPIO19 node → 3.3 kΩ → GND**. Join the sensor ground to ESP32 GND. Do not connect ECHO directly to the ESP32.

## Firmware

Open [src/interactive_robot.ino](src/interactive_robot.ino) in Arduino IDE, select **ESP32 Dev Module**, choose the correct serial port, and upload. No extra Arduino library is required.

The firmware starts stopped. Press the button to toggle the robot. The obstacle threshold is set to 25 cm in the source. If the robot turns the wrong way, swap a motor’s leads or reverse its direction in the firmware.

For the first bench run, raise the wheels so they cannot drive the chassis off the table. Disconnect motor power before changing wiring. The reference firmware drives motors at full duty; it does not provide speed control.

## Important limitations

- This is an educational robot reference, not a tested record of the original build.
- Pin assignments assume a classic ESP32 DevKit and the listed breakout labels.
- HC-SR04 measurements and obstacle behavior depend on the sensor, surface, motor polarity, and chassis.
- Confirm each motor’s stall current and supply voltage against the actual driver board. Do not power motors from ESP32 GPIO or 3V3.
- The diagram is a wiring reference, not a PCB manufacturing schematic.

## References

- [Toshiba TB6612FNG product page and datasheet](https://toshiba.semicon-storage.com/ap-en/semiconductor/product/motor-driver-ics/brushed-dc-motor-driver-ics/detail.TB6612FNG.html)
- [HC-SR04 datasheet download](https://learn.adafruit.com/ultrasonic-sonar-distance-sensors/downloads)

## Author

Abinavu M — Electronics and Communication Engineering student focused on embedded systems and IoT.
