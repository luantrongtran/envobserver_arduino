
/**
 * Configuring the paths for APIs
 */
void configRouting() {
  Serial.println("Configuring Router for Soft AP");
  http_rest_server.on("/", showInfo);
  http_rest_server.on("/activate", activateDevice);
  http_rest_server.on("/wifi", updateWifi);
}

/**
 * Setup Esp8266 as an Soft Access Point
 */
void setupesp8266AccessPoint() {
  WiFi.softAP(ACCESS_POINT_SSID, ACCESS_POINT_PASS);
  http_rest_server.begin();
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  configRouting();
}

/**
 * This is to activate the device
 */
void activateDevice() {
  if (isActivated() == true) {
    http_rest_server.send(400, "text/plain", "failed: device already activated");
    return;
  }

  String paramNameOwnerId = "ownerId";

  sendCorsHeader();
  if(http_rest_server.hasArg(paramNameOwnerId) == false) {
    http_rest_server.send(400, "text/plain", "Missing userId who owns this device");
  }
  
  String ownerId = http_rest_server.arg(paramNameOwnerId);
  activateDeviceOnline(ownerId);
  http_rest_server.send(200, "text/plain", "success");
}

/**
 * UPdate Wifi Info API
 */
void updateWifi() {
  Serial.println("Update wifi handler");
  sendCorsHeader();

  String paramWifiName = "ssid";
  String paramWifiPass = "pass";

  String errMsg = "{\"msg\": \"Wifi SSID (name) must not be empty\"}";
  String wifiName = "";
  String wifiPass = "";
  if(http_rest_server.hasArg(paramWifiName) == false) {
    Serial.println(errMsg);
    http_rest_server.send(400, "application/json", errMsg);
    return;
  } else {
    wifiName = http_rest_server.arg(paramWifiName);
    if(wifiName == "") {
      http_rest_server.send(400, "application/json", errMsg);  
    }
  }

  errMsg = "{\"msg\": \"Wifi Password must not be empty\"}";
  if(http_rest_server.hasArg(paramWifiPass) == false) {
    Serial.println(errMsg);
    http_rest_server.send(400, "application/json", errMsg);
    return;
  } else {
    wifiPass = http_rest_server.arg(paramWifiPass);
    if(wifiPass == "") {
      http_rest_server.send(400, "application/json", errMsg);
    }
  }

  config.wifiName = wifiName;
  config.wifiPass = wifiPass;

  bool b = saveConfig();

  if (b == true) {
    String msg = "{\"msg\": \"Wifi updated, need to restart the device\"}";
    http_rest_server.send(200, "application/json", msg);
  } else {
    String msg = "{\"msg\": \"failed to update wifi\"}";
    http_rest_server.send(500, "application/json", msg);
  }
}

void showInfo() {
  StaticJsonDocument<200> doc;

  // ineternet settings
  JsonObject internetSettings  = doc.createNestedObject("internet");
  if (WiFi.status() == WL_CONNECTED) {
    internetSettings["internetStatus"] = true;
  } else {
    internetSettings["internetStatus"] = false;
  }
  internetSettings["wifiSsid"] = config.wifiName;

  // device config
  JsonObject deviceConfig  = doc.createNestedObject("deviceConfig");
  deviceConfig["deviceName"] = config.deviceName;
  deviceConfig["deviceId"] = config.deviceId;
  deviceConfig["timeOffset"] = config.timeOffset;
  deviceConfig["uploadInterval"] = config.pollingInterval;
  
  String temp;
  serializeJson(doc, temp);
  sendCorsHeader();
  http_rest_server.send(200, "application/json", temp);
}

/**
 * This is to send headers for CORS
 */
void sendCorsHeader() {
  http_rest_server.sendHeader("Access-Control-Allow-Origin", "*");
  http_rest_server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
}
