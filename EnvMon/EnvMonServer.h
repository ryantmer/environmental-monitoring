#ifndef EnvMonServer_h
#define EnvMonServer_h

#include "ESPAsyncWebServer.h"

AsyncWebServer server(80);
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

void initServer() {
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
}

#endif