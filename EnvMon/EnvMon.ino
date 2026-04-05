#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include "ESPAsyncWebServer.h"
#include "time.h"

#include <EnvMonWiFi.h>

#define PIN_DHT 1
#define PIN_SCREEN_ENABLE 2
#define PIN_SCREEN_BUTTON 3

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C // 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ntpServer = "pool.ntp.org";
const long gmtOffsetSeconds = 0;
const int daylightOffsetSeconds = 0;
String timezone = "PST8PDT,M3.2.0,M11.1.0";
struct tm timeinfo;

#define DHT_TYPE DHT11
DHT dht(PIN_DHT, DHT_TYPE);

AsyncWebServer server(80);

String readTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }

  return String(t);
}

String readHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }

  return String(h);
}

String getTimestamp() {
  if (getLocalTime(&timeinfo)) {
    char timeBuf[20];
    strftime(timeBuf, sizeof(timeBuf), "%FT%T", &timeinfo);
    String asString(timeBuf);
    return timeBuf;
  } else {
    Serial.println("Failed to fetch time");
    display.println("Failed to fetch time");
    return "--";
  }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
  </style>
</head>
<body>
  <h2>Environmental Monitoring</h2>
  <p>%TIMESTAMP%</p>
  <p>Temperature: <span id="temperature">%TEMPERATURE%</span>&deg;C</p>
  <p>Humidity: <span id="humidity">%HUMIDITY%</span>&percnt;</p>
</body>
<script>
  const getData = async(dataType) => {
    const response = await fetch(`/${dataType}`);
    const datapoint = await response.text();
    document.getElementById(dataType).innerHTML = datapoint;
  }

  setInterval(getData.apply("temperature"), 10000);
  setInterval(getData.apply("humidity"), 10000);
</script>
</html>)rawliteral";
String processor(const String& var){
  if(var == "TEMPERATURE") {
    return readTemperature();
  } else if(var == "HUMIDITY") {
    return readHumidity();
  } else if(var == "TIMESTAMP") {
    return getTimestamp();
  }
  return String();
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

void updateDisplay() {
  display.clearDisplay();

  String timestamp = getTimestamp();
  String t = readTemperature();
  String h = readHumidity();
  String temperature = "Temperature: " + t + "C";
  String humidity = "Humidity: " + h + "%";
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

void setup() {
  Serial.begin(115200);

  initWiFi();
  
  configTime(gmtOffsetSeconds, daylightOffsetSeconds, ntpServer);
  setenv("TZ", timezone.c_str(), 1);
  tzset();
  Serial.print("Got time from ");
  Serial.println(ntpServer);

  dht.begin();
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readHumidity().c_str());
  });

  // Start server
  server.begin();

  initDisplay();
  updateDisplay();
  delay(3000);
}

void loop() {
  if (checkScreenOn()) {
    initDisplay();
    for (int i = 0; i < 10; i++) {
      updateDisplay();
      delay(1000);
    }
  }
  delay(25);
}
