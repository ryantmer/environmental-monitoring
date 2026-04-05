#ifndef EnvMonTime_h
#define EnvMonTime_h

#include "time.h"

const char* ntpServer = "pool.ntp.org";
const long gmtOffsetSeconds = 0;
const int daylightOffsetSeconds = 0;
String timezone = "PST8PDT,M3.2.0,M11.1.0";
struct tm timeinfo;

void initTime() {  
  configTime(gmtOffsetSeconds, daylightOffsetSeconds, ntpServer);
  setenv("TZ", timezone.c_str(), 1);
  tzset();
  Serial.print("Got time from ");
  Serial.println(ntpServer);
}

String getTimestamp() {
  if (getLocalTime(&timeinfo)) {
    char timeBuf[20];
    strftime(timeBuf, sizeof(timeBuf), "%FT%T", &timeinfo);
    String asString(timeBuf);
    return timeBuf;
  } else {
    Serial.println("Failed to fetch time");
    return "Failed to fetch time";
  }
}

#endif