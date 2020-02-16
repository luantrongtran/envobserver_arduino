const int CONFIG_SIZE = 200;

void setupConfigFile() {
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
}

bool saveConfig(Config config) {
  StaticJsonDocument<CONFIG_SIZE> doc;
  doc["wifiName"] = config.wifiName;
  doc["wifiPass"] = "12345";

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  serializeJson(doc, configFile);
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

  StaticJsonDocument<200> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse config file");
    return false;
  }

  const char* wifiName = doc["wifiName"];
  const char* wifiPass = doc["wifiPass"];

  // Real world application would store these values in some variables for
  // later use.

  Serial.print("Loaded serverName: ");
  Serial.println(wifiName);
  Serial.print("Loaded accessToken: ");
  Serial.println(wifiPass);
  return true;
}

StaticJsonDocument<CONFIG_SIZE> serializeConfig(Config config_) {
  StaticJsonDocument<CONFIG_SIZE> doc;
  
  if (config_.wifiName != NULL) {
    doc["wifiName"] = config_.wifiName;
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

