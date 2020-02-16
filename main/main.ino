#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

#include <Wire.h>
#include <RtcDS3231.h>
#include <ArduinoJson.h>

#include "FS.h"
/*
 * Method Declarations
 */
void post(String, String);
void blink(int);
void doubleBlink(int);
void uploadData();
String formatIso8601(RtcDateTime);
void loadDefaultConfig();
void tripleBlink(int);
int readSoilSensor();

void configRouting();
void setupesp8266AccessPoint();

void setupEsp8266Client();
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
#define DEFAULT_API_SERVER "192.168.1.6:3000"
//#define DEFAULT_POLLING_INTERVAL 60000
//#define DEFAULT_API_SERVER "54.254.164.155"

//#define DEFAULT_API_SERVER "192.168.1.42"

#define WIFI_LED_PIN 2

#define DEFAULT_TIME_OFFSET "+07:00"

/*
 * Rest Server's wifi name and password
 */
#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50
const char* ACCESS_POINT_SSID = "ENVOBSERVER_NOT_REGISTERED";
const char* ACCESS_POINT_PASS = "00000000";

ESP8266WebServer http_rest_server(HTTP_REST_PORT);

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
  loadDefaultConfig();
  Serial.begin(115200);

  setupConfigFile();

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

//  setupEsp8266Client();
  setupesp8266AccessPoint();

}

void loop() {
  // polling interval
  delay(config.pollingInterval);

  // upload temperature data to server
//  uploadData();

  http_rest_server.handleClient();
//  blink(500);
}



int readSoilSensor() {
  int sensorValue = analogRead(ANALOG_SENSOR_PIN);
  return sensorValue;
}



