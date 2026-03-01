#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <WiFi.h>
#include <Wire.h>
#include "ESPAsyncWebServer.h"
#include "time.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "network name";
const char* password = "password";
IPAddress localIP(192, 168, 1, 50);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 2);

const char* ntpServer = "pool.ntp.org";
const long gmtOffsetSeconds = 0;
const int daylightOffsetSeconds = 3600;
struct tm timeinfo;

#define DHT_PIN 1
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

AsyncWebServer server(80);
String readDHTTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

String readDHTHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}

String getTimestamp() {
  if (getLocalTime(&timeinfo)) {
    char timeBuf[20];
    strftime(timeBuf, sizeof(timeBuf), "%FT%T", &timeinfo);
    String asString(timeBuf);
    Serial.println(timeBuf);
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
    return readDHTTemperature();
  } else if(var == "HUMIDITY") {
    return readDHTHumidity();
  } else if(var == "TIMESTAMP") {
    return getTimestamp();
  }
  return String();
}

void setupWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.config(localIP, gateway, subnet, dns);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println(WiFi.localIP());

  // WiFi.disconnect(true);
  // WiFi.mode(WIFI_OFF);
}

void setup() {
  Serial.begin(115200);

  dht.begin();

  Serial.println("Waiting for display...");
  delay(500);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  Serial.println("Display found.");
  delay(500);

  setupWiFi();
  
  configTime(gmtOffsetSeconds, daylightOffsetSeconds, ntpServer);
  Serial.print("Got time from ");
  Serial.println(ntpServer);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });

  // Start server
  server.begin();
}

void loop() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(getTimestamp());
  display.println(WiFi.localIP());
  display.println("Temperature: " + readDHTTemperature() + "C");
  display.println("Humidity: " + readDHTHumidity() + "%");

  display.display();

  delay(10000);
}
