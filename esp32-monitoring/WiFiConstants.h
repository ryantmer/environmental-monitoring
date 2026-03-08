#ifndef WiFiConstants_h
#define WiFiConstants_h

const char* ssid = "network name";
const char* password = "password";

IPAddress localIP(192, 168, 1, 50);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);

#endif