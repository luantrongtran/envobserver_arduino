
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
  http_rest_server.send(200, "text/plain", "success");
}

/**
 * UPdate Wifi Info API
 */
void updateWifi() {
  Serial.println("Update wifi handler");
  
  String message = "Body received:\n";
             message += http_rest_server.arg("plain");
             message += "\n";
   Serial.println(message);
  http_rest_server.send(200, "text/plain", message);

}

void showInfo() {
  blink(100);
  Serial.println("AP handler");
  http_rest_server.send(200, "text/html", "<h1>Hello from ESP8266 AP 123!</h1>");
}

