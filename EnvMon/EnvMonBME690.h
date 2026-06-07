#ifndef EnvMonBME690_h
#define EnvMonBME690_h

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#define BME690_ADDRESS 0x76
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme(&Wire);

void initBME() {
  if (!bme.begin(BME690_ADDRESS)) {
    Serial.println("Could not find BME680 sensor, check wiring");
    while (1);
  }
}

void measureBME() {
  // unsigned long endTime = bme.beginReading();
  // if (endTime == 0) {
  //   Serial.println("Failed to begin reading");
  //   return;
  // }
  // if (!bme.endReading()) {
  //   Serial.println("Failed to complete reading");
  //   return;
  // }
  bme.performReading();
  Serial.println("Temperature = " + String(bme.temperature) + "ºC");
  Serial.println("Pressure = " + String(bme.pressure / 1000.0) + "kPa");
  Serial.println("Humidity = " + String(bme.humidity) + "%");
  Serial.println("Gas = " + String(bme.gas_resistance / 1000.0) + "KOhms");
  Serial.println("Approx. Altitude = " + String(bme.readAltitude(SEALEVELPRESSURE_HPA)) + "m");
  Serial.println();
  delay(2000);
}

#endif
