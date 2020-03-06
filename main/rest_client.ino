/**
 * This is to connect the device to Internet
 */
void connectWifi(String ssid, String password)  {
  Serial.println("Connecting wifi");
  int retry = 0;

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    // enable built in led
    doubleBlink(100);

    if (retry == WIFI_CONNECTION_RETRY) {
      break;
    }

    retry++;
    delay(1500);
  }

  Serial.println(WiFi.status());

  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("Wifi connected");
  }

  // after successfully wifi-connected
  digitalWrite(WIFI_LED_PIN, LOW);
}

void setupEsp8266Client(){
  // Connect to Internet to send temperature data
  connectWifi(config.wifiName, config.wifiPass);
}

bool activateDeviceOnline(String ownerId) {
  String apiServer = "";//"http://";
  apiServer += config.apiServer;
  apiServer += "/envobservers/activate";
  Serial.println(apiServer);

  StaticJsonDocument<CONFIG_SIZE> body;
  body["userId"] = ownerId;
  body["deviceName"] = config.deviceName;
  
  JsonObject options  = body.createNestedObject("options");
  options["pollingInterval"] = config.pollingInterval;
  options["timeOffset"] = config.timeOffset;
  
  String reqBody;
  serializeJson(body, reqBody);
  Serial.println(reqBody);
//  reqBody += ownerId;
//  reqBody += "\"}";
  
  String payload = post(reqBody, apiServer);
  Serial.println(payload);
  if (payload == "-1") {
    "Failed to connect to backend server";
    return false;  
  }
  char temp[200];
  payload.toCharArray(temp, sizeof(temp));
  StaticJsonDocument<CONFIG_SIZE> doc;
  auto error = deserializeJson(doc, temp);

  if(error) {
    Serial.println("Activate API: Failed to parse returned payload");
    return false;
  }

  String deviceId = doc["_id"];
  config.deviceId = deviceId;
  config.userId = ownerId;
  saveConfig();

  return true;
}

/**
 * Sending temperature, humidity and soil moisture to backend
 */
void uploadData() {

  // variable to store temperature and humidity event
  sensors_event_t event;

  // get temperature
  dht.temperature().getEvent(&event);
  float temperature = event.temperature;
//  Serial.println(temperature);

  // get humidity event
  dht.humidity().getEvent(&event);
  float humidity = event.relative_humidity;
//  Serial.println(humidity);

  // get soil moisture
  int soilMoisture = readSoilSensor();
//  Serial.println(soilMoisture);
  
  RtcDateTime now = rtc.GetDateTime();
    printDateTime(now);
    Serial.println();
  Serial.print( now.Epoch32Time() ) ;
  String strRecordedAt = formatIso8601(now);

  DynamicJsonDocument doc(1024);
  doc["deviceId"] = config.deviceId;
  JsonObject data  = doc.createNestedObject("data");
  data["temperature"] = temperature;
  data["humidity"] = humidity;
  data["soil_moisture"] = soilMoisture;
  data["recorded_at"] = strRecordedAt;

  String body;
  serializeJson(doc, body);
  
  Serial.println(body);

  String apiServer = "";//"http://";
  apiServer += config.apiServer;
  apiServer += "/envobservers/uploaddata";
  Serial.println(apiServer);
  
  post(body, apiServer);
}



/**
 * Send a json to backend
 * 
 * @apiServer is the url of the backend
 */ 
String post(String json, String apiServer) {
  WiFiClient client;
  HTTPClient http;
  
//  http.begin(client, DEFAULT_API_SERVER "/envobserver");
  
  http.begin(client, apiServer); //HTTP
  http.addHeader("Content-Type", "application/json");

  Serial.println(F("Sending request ..."));
  int httpCode = http.POST(json);
  const String& payload = http.getString();
  Serial.println(httpCode);
  if (httpCode == HTTP_CODE_OK) {
      return payload;
  } else {
    Serial.println(payload);
    return "-1";
  }
}

int readSoilSensor() {
  int sensorValue = analogRead(ANALOG_SENSOR_PIN);
  return sensorValue;
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}
