#ifndef EnvMonWiFi_h
#define EnvMonWiFi_h

#include <WiFi.h>
#include <EnvMonSecrets.h>

const char* ssid = SECRET_WIFI_SSID;
const char* password = SECRET_WIFI_PASSWORD;

IPAddress localIP(192, 168, 1, 50);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);

void initWiFi() {
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
}

IPAddress getLocalIP() {
  return WiFi.localIP();
}

#endif