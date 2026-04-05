#include <EnvMonWiFi.h>
#include <EnvMonTime.h>
#include <EnvMonDHT11.h>
#include <EnvMonDisplay.h>
#include <EnvMonServer.h>

void setup() {
  Serial.begin(115200);

  initWiFi();
  initTime();
  initDHT11();
  initDisplay();
  initServer();

  updateDisplay(getTimestamp(), readTemperature(), readHumidity());

  delay(3000);
}

void loop() {
  if (checkScreenOn()) {
    // Need to re-init display any time it is powered back on
    initDisplay();
    for (int i = 0; i < 10; i++) {
      updateDisplay(getTimestamp(), readTemperature(), readHumidity());
      delay(1000);
    }
  }
  delay(25);
}
