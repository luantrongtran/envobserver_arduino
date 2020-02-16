
void setupEsp8266Client(){
  // Connect to Internet to send temperature data
  WiFi.begin(config.wifiName, config.wifiPass);
  while (WiFi.status() != WL_CONNECTED) {
    // enable built in led
    doubleBlink(100);
    delay(1500);
  }

  // after successfully wifi-connected
  digitalWrite(WIFI_LED_PIN, LOW);
  Serial.println("connected");
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
  String strRecordedAt = formatIso8601(now);

  DynamicJsonDocument doc(1024);
  doc["deviceId"] = deviceId;
  JsonObject data  = doc.createNestedObject("data");
  data["temperature"] = temperature;
  data["humidity"] = humidity;
  data["soil_moisture"] = soilMoisture;
  data["recorded_at"] = strRecordedAt;

  String body;
  serializeJson(doc, body);
  
  Serial.println(body);

  String apiServer = "http://";
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
void post(String json, String apiServer) {
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
      blink(1000);
  } else {
    // triple blink if it failed to send data to API server
    tripleBlink(200);
  }
}
