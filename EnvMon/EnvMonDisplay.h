#ifndef EnvMonDisplay_h
#define EnvMonDisplay_h

#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define PIN_SCREEN_ENABLE 2
#define PIN_SCREEN_BUTTON 3
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C // 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initDisplay() {
  pinMode(PIN_SCREEN_ENABLE, OUTPUT);
  pinMode(PIN_SCREEN_BUTTON, INPUT);
  digitalWrite(PIN_SCREEN_ENABLE, HIGH);

  delay(100);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    return;
  }
}

int lastButtonState = 0;
bool checkScreenOn() {
  int buttonState = digitalRead(PIN_SCREEN_BUTTON);
  if (buttonState == lastButtonState) {
    return false;
  }

  lastButtonState = buttonState;
  if (buttonState == LOW) {
    Serial.println("Screen on");
    digitalWrite(PIN_SCREEN_ENABLE, HIGH);
    return true;
  } else {
    Serial.println("Screen off");
    digitalWrite(PIN_SCREEN_ENABLE, LOW);
    return false;
  }
}

void updateDisplay(String timestamp, float temperature, float humidity) {
  display.clearDisplay();

  String t = "Temperature: " + String(temperature) + "C";
  String h = "Humidity: " + String(humidity) + "%";
  Serial.println(timestamp + "; " + temperature + "; " + humidity);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(timestamp);
  display.println(getLocalIP());
  display.println(temperature);
  display.println(humidity);

  display.display();
}

#endif