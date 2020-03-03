void setupConfigFile() {
  // 1st load default config
  loadDefaultConfig();
    
  Serial.println("Mounting FS...");
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

//  bool b = saveConfig();
//  Serial.print("Saving config returns: ");
//  Serial.println(b);

  bool b= loadConfigFile();
  Serial.print("Loading config returns: ");
  Serial.println(b);
  printConfig();
}

void printConfig() {
  Serial.println(config.wifiName);
  //Serial.println(config.wifiPass);
  Serial.println(config.deviceId);
}

bool saveConfig() {
  Serial.println("Saving new config");
  StaticJsonDocument<CONFIG_SIZE> doc;
  doc = serializeConfig(config);

  if (isEmptyOrNull(config.wifiName) == true) {
    doc["wifiName"] = config.wifiName;
  }

  if (isEmptyOrNull(config.wifiPass) == true) {
    doc["wifiPass"] = config.wifiPass;
  }

  if (isEmptyOrNull(config.deviceId)) {
    doc["deviceId"] = config.deviceId;
  }

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  serializeJson(doc, configFile);

  printConfig();
  return true;
}

bool loadConfigFile() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<CONFIG_SIZE> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse config file");
    return false;
  }

  // Load config into the global variable config
  if (doc.containsKey("deviceId") == true) {
    const char* deviceId = doc["deviceId"];
    config.deviceId = String(deviceId);
  }
  
  if(doc.containsKey("wifiName") == true) {
    const char* wifiName = doc["wifiName"];
    config.wifiName = String(wifiName);  
  }

  if (doc.containsKey("wifiPass") == true) {
    const char* wifiPass = doc["wifiPass"];
    config.wifiPass = String(wifiPass);
  }

  return true;
}

StaticJsonDocument<CONFIG_SIZE> serializeConfig(Config config_) {
  StaticJsonDocument<CONFIG_SIZE> doc;

  if (isEmptyOrNull(config.wifiName) == true) {
    doc["wifiName"] = config.wifiName;
  }

  if (config_.wifiPass != NULL) {
    doc["wifiPass"] = config_.wifiPass;
  }

  return doc;
}

void loadDefaultConfig() {
  Serial.println("Loading default config...");
  // Load default configurations
  config.deviceName = DEFAULT_DEVICE_NAME;
  config.wifiName = DEFAULT_WIFI_NAME;
  config.wifiPass = DEFAULT_WIFI_PASS;
  config.pollingInterval = DEFAULT_POLLING_INTERVAL;
  config.apiServer = DEFAULT_API_SERVER;

  config.timeOffset = DEFAULT_TIME_OFFSET;
}

void removeConfigFile() {
  Serial.println("Removing config file");
  String filePath = "/config.json";
  if (SPIFFS.exists(filePath) == false) {
    return;
  }
  SPIFFS.remove(filePath);
}

bool isActivated() {
  if (config.deviceId != NULL && config.deviceId != "") {
    return true;
  }

  return false;
}
