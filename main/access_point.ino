
void setupesp8266AccessPoint() {
  WiFi.softAP(ACCESS_POINT_SSID, ACCESS_POINT_PASS);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}
