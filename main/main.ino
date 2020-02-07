#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <Wire.h>
#include <RtcDS3231.h>
#include <ArduinoJson.h>
/*
 * Method Declarations
 */
void post(String);
void blink(int);
void doubleBlink(int);
void uploadData();
String formatIso8601(RtcDateTime);
void loadConfig();
void tripleBlink(int);
int readSoilSensor();

/**
 * struct
 */

struct config {
  String deviceName;
  
  // the password when the eps8266 acts as a server
  String selfServerPass;

  // the wifi name and password when esp8266 acts as a client
  String wifiName;
  String wifiPass;

  // How often to send data to the server
  int pollingInterval;

  // This is the url of the API server
  String apiServer;

  String timeOffset;
};
typedef struct config Config;

/*
 * defines
 */

#define DEFAULT_DEVICE_NAME "UNKNOWN"

#define DEFAULT_WIFI_NAME "TrongUyen"
#define DEFAULT_WIFI_PASS "1943LamSon"
 
#define DHTPIN D3
#define DHTTYPE DHT11

#define ANALOG_SENSOR_PIN A0

// how often send data to server, in ms
#define DEFAULT_POLLING_INTERVAL 5000
#define DEFAULT_API_SERVER "192.168.1.2:3000"
//#define DEFAULT_API_SERVER "192.168.1.42"

#define WIFI_LED_PIN 2

#define DEFAULT_TIME_OFFSET "+10:00"

/*
 * Glbal varibles 
 */ 
DHT_Unified dht(DHTPIN, DHTTYPE);
Config config;

String deviceId = "1";

RtcDS3231<TwoWire> rtc(Wire);

void setup() {
  Serial.println('Starting ...');
  // put your setup code here, to run once:
  loadConfig();
  Serial.begin(115200);

  // enable Temperature sensor
  dht.begin();

  // entable DS3231 - real time clock
  rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  if (!rtc.IsDateTimeValid()) 
  {
      Serial.println("RTC lost confidence in the DateTime!");
      rtc.SetDateTime(compiled);
  }
  rtc.Enable32kHzPin(false);
  rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);

  // Setup pin for LEDs
  pinMode(WIFI_LED_PIN, OUTPUT);
  digitalWrite(WIFI_LED_PIN, HIGH);

  // Connect to Wifi to send temperature data
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

void loop() {
  // polling interval
  delay(config.pollingInterval);

  // upload temperature data to server
  uploadData();

}

void uploadData() {

  // variable to store temperature and humidity event
  sensors_event_t event;

  // get temperature
  dht.temperature().getEvent(&event);
  float temperature = event.temperature;
  Serial.println(temperature);

  // get humidity event
  dht.humidity().getEvent(&event);
  float humidity = event.relative_humidity;
  Serial.println(humidity);

  // get soil moisture
  int soilMoisture = readSoilSensor();
  Serial.println(soilMoisture);
  
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
  post(body);
}

void loadConfig() {
  Serial.println("Loading config...");
  // Load default configurations
  config.deviceName = DEFAULT_DEVICE_NAME;
  config.wifiName = DEFAULT_WIFI_NAME;
  config.wifiPass = DEFAULT_WIFI_PASS;
  config.pollingInterval = DEFAULT_POLLING_INTERVAL;
  config.apiServer = DEFAULT_API_SERVER;

  config.timeOffset = DEFAULT_TIME_OFFSET;

}

void post(String json) {
  WiFiClient client;
  HTTPClient http;
  
//  http.begin(client, DEFAULT_API_SERVER "/envobserver");
  String apiServer = "http://";
  apiServer += config.apiServer;
  apiServer += "/envobservers/uploaddata";
  http.begin(client, apiServer); //HTTP
  http.addHeader("Content-Type", "application/json");

  Serial.println(F("Sending request ..."));
  int httpCode = http.POST(json);
  const String& payload = http.getString();
  if (httpCode == HTTP_CODE_OK) {
      blink(1000);
  } else {
    // triple blink if it failed to send data to API server
    tripleBlink(200);
  }
}

int readSoilSensor() {
  int sensorValue = analogRead(ANALOG_SENSOR_PIN);
  return sensorValue;
}

/*
 * Blink led on WIFI_LED_PIN
 */
void blink(int interval) {
  digitalWrite(WIFI_LED_PIN, LOW);
  delay(interval);
  digitalWrite(WIFI_LED_PIN, HIGH);
}

/*
 * Double blink led on WIFI_LED_PIN
 */
void doubleBlink(int interval) {
  blink(interval);
  delay(interval);
  blink(interval);
}

void tripleBlink(int interval) {
  blink(interval);
  delay(interval);
  blink(interval);
  delay(interval);
  blink(interval);
}

/**
 * Format a date into ISO8601 string
 */
String formatIso8601(RtcDateTime date) {
  String month = "";
  month = date.Month();
  Serial.println(month);
//  Serial.println("Month: " + date.Month());
  if (date.Month() < 10) {
    month = "0" + month;
  }
  
  String day ;
  day= date.Day();
  if (date.Day() < 10) {
    day = "0" + day;
  }
  
  String hour ;
  hour = date.Hour();
  if (date.Hour() < 10) {
    hour = "0" + hour;
  }

  String minute;
  minute = date.Minute();
  if(date.Minute() < 10) {
    minute = "0" + minute;
  }

  String second;
  second = date.Second();
  if (date.Second() < 10) {
    second = "0" + second;
  }

  String str;
  str = date.Year() ;
  str += "-";
  str += month;
  str += "-";
  str += day;
  str += "T";
  str += hour;
  str += ":";
  str += minute;
  str += ":";
  str += second;

  str += config.timeOffset;

  return str;
}
