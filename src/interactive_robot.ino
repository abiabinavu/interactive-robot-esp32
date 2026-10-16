/*
 * Interactive ESP32 robot: local touch-response reference scaffold.
 * Demonstrates friendly OLED face and servo shake. Voice capture, speech
 * recognition, ChatGPT, and spoken answers require the separate secure
 * backend/audio integration described in README.md.
 * Assumed hardware; generated example, not hardware-verified.
 */
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

constexpr uint8_t TOUCH_PIN=27, SERVO_PIN=18, OLED_SDA=21, OLED_SCL=22;
constexpr uint8_t OLED_ADDR=0x3C;
Adafruit_SSD1306 display(128,64,&Wire,-1);
Servo reactionServo;
bool wasTouched=false;

void face(const char *caption) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.drawCircle(64,28,22,SSD1306_WHITE);
  display.fillCircle(56,23,2,SSD1306_WHITE);
  display.fillCircle(72,23,2,SSD1306_WHITE);
  display.drawLine(54,36,59,40,SSD1306_WHITE);
  display.drawLine(59,40,64,42,SSD1306_WHITE);
  display.drawLine(64,42,69,40,SSD1306_WHITE);
  display.drawLine(69,40,74,36,SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0,56);
  display.print(caption);
  display.display();
}
void shake() {
  for (uint8_t i=0;i<3;i++) {
    reactionServo.write(72); delay(120);
    reactionServo.write(108); delay(120);
  }
  reactionServo.write(90);
}
void setup() {
  Serial.begin(115200);
  pinMode(TOUCH_PIN,INPUT);
  Wire.begin(OLED_SDA,OLED_SCL);
  if (display.begin(SSD1306_SWITCHCAPVCC,OLED_ADDR)) face("Hello! Ask me a question.");
  else Serial.println("Check OLED wiring/address.");
  reactionServo.setPeriodHertz(50);
  reactionServo.attach(SERVO_PIN,500,2400);
  reactionServo.write(90);
}
void loop() {
  bool touched=digitalRead(TOUCH_PIN)==HIGH;
  if (touched && !wasTouched) {
    face("<3 Happy!");
    shake();
  }
  wasTouched=touched;
  delay(20);
}
/*
 * Voice/ChatGPT integration outline (not implemented here):
 * 1. Capture question audio from an I2S microphone.
 * 2. Send over TLS to a trusted backend.
 * 3. Backend performs speech-to-text, ChatGPT, and text-to-speech.
 * 4. Stream answer audio to an I2S amplifier/speaker.
 * Never commit API keys or Wi-Fi credentials to this public repository.
 */