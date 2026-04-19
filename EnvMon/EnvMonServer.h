#ifndef EnvMonServer_h
#define EnvMonServer_h

#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);
String historicalTimestamps[24];
float historicalTemperature[24];
float historicalHumidity[24];
unsigned int lastRunHour = 0;

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

    table { border-collapse: collapse; }
    tr { border: none; }
    td, th {
      border-right: 1px solid black;
      border-left: 1px solid black;
      padding-inline: 10px;
    }
  </style>
</head>
<body>
  <h2>Environmental Monitoring</h2>
  <p>At: <span id="timestamp">%TIMESTAMP%</span></p>
  <p>Temperature: <span id="temperature">%TEMPERATURE%</span>&deg;C</p>
  <p>Humidity: <span id="humidity">%HUMIDITY%</span>&percnt;</p>
  <table id="history">
    <tr>
      <th scope="col">Time</th>
      <th scope="col">Temperature</th>
      <th scope="col">Humidity</th>
    </tr>
  </table>
</body>
<script>
  const parseValue = val => (!val || val <= 0) ? '--' : val;
  const appendDataCell = (tr, data) => {
    const td = document.createElement('td');
    td.textContent = parseValue(data);
    tr.appendChild(td);
  }
  const getData = async() => {
    const response = await fetch('/data');
    const json = await response.json();

    document.getElementById('timestamp').innerHTML = json.now.timestamp;
    document.getElementById('temperature').innerHTML = parseValue(json.now.temperature);
    document.getElementById('humidity').innerHTML = parseValue(json.now.humidity);

    const newDataRows = [];
    for (let i = 0; i < 24; i++) {
      const tr = document.createElement('tr');

      const time = document.createElement('th');
      time.setAttribute("scope", "row");
      time.textContent = `${i + 1}h ago (${json['timestamps'][i]})`;
      tr.appendChild(time);

      appendDataCell(tr, json['temperatures'][i]);
      appendDataCell(tr, json['humidities'][i]);

      newDataRows.push(tr);
    }

    const table = document.getElementById('history');
    const headerRow = table.querySelector('tr:first-child');
    table.replaceChildren(headerRow, ...newDataRows);
  };
  getData();

  setInterval(() => getData(), 10000);
</script>
</html>
)rawliteral";
String processor(const String& var){
  if(var == "TEMPERATURE") {
    return String(readTemperature(), 2);
  } else if(var == "HUMIDITY") {
    return String(readHumidity(), 2);
  } else if(var == "TIMESTAMP") {
    return getTimestamp();
  }
  return String();
}

void updateData() {
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  // Update historical data every hour, on the hour
  if (timeinfo.tm_hour != lastRunHour && timeinfo.tm_min == 0) {
    Serial.println("Time to update historical data.");

    // Shift historical data
    for (int i = 23; i > 0; i--) {
      historicalTimestamps[i] = historicalTimestamps[i-1];
      historicalTemperature[i] = historicalTemperature[i-1];
      historicalHumidity[i] = historicalHumidity[i-1];
    }

    // Add new datapoints to start of arrays
    historicalTimestamps[0] = getTimestamp();
    historicalTemperature[0] = readTemperature();
    historicalHumidity[0] = readHumidity();
    lastRunHour = timeinfo.tm_hour;
  }
}

void initServer() {
  // Route for root / web page
  server.on(AsyncURIMatcher::exact("/"), HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html, processor);
  });

  server.on(AsyncURIMatcher::exact("/data"), HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    JsonDocument doc;

    doc["now"]["timestamp"] = getTimestamp();
    doc["now"]["temperature"] = readTemperature();
    doc["now"]["humidity"] = readHumidity();

    for (int hour = 0; hour < 24; hour++) {
      doc["timestamps"][hour] = historicalTimestamps[hour];
      doc["temperatures"][hour] = historicalTemperature[hour];
      doc["humidities"][hour] = historicalHumidity[hour];
    }
    serializeJson(doc, *response);
    request->send(response);
  });

  // Start server
  server.begin();

  updateData();
}

#endif