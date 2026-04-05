#ifndef EnvMonDHT11_h
#define EnvMonDH11_h

#include <DHT.h>
#include <Wire.h>

#define PIN_DHT 1
#define DHT_TYPE DHT11

DHT dht(PIN_DHT, DHT_TYPE);

void initDHT11() {
  dht.begin();
}

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

#endif