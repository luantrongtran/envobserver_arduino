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

/**
 * 0 : WL_IDLE_STATUS when Wi-Fi is in process of changing between statuses
1 : WL_NO_SSID_AVAILin case configured SSID cannot be reached
3 : WL_CONNECTED after successful connection is established
4 : WL_CONNECT_FAILED if password is incorrect
6 : WL_DISCONNECTED if module is not configured in station mode
 * 
 */

/*
 * defines
 */

#define DEFAULT_DEVICE_NAME "UNKNOWN"

#define DEFAULT_WIFI_NAME "1"
#define DEFAULT_WIFI_PASS "1"
 
#define DHTPIN D3
#define DHTTYPE DHT11
//#define DHTTYPE DHT22

#define ANALOG_SENSOR_PIN A0

// how often send data to server, in ms
#define DEFAULT_POLLING_INTERVAL 10000
//#define DEFAULT_API_SERVER "http://192.168.1.7:3000"
//#define DEFAULT_POLLING_INTERVAL 60000
#define DEFAULT_API_SERVER "http://54.254.164.155"

//#define DEFAULT_API_SERVER "192.168.1.42"

#define WIFI_LED_PIN 2

#define DEFAULT_TIME_OFFSET "+07:00"

/*
 * Rest Server's wifi name and password
 */
#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50

const int CONFIG_SIZE = 200;
const char* ACCESS_POINT_SSID = "ENVOBSERVER_NOT_REGISTERED";
const char* ACCESS_POINT_PASS = "00000000";
const int WIFI_CONNECTION_RETRY = 5;

/**
 * struct
 */

struct config {
  // the owner
  String userId;
  String deviceId;
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
 * Method Declarations
 */
String post(String, String);
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
bool isEmptyOrNull(String str);

void setupRtcModule();

StaticJsonDocument<CONFIG_SIZE> serializeConfig(Config);

// config_service;
void printConfig();

/*
 * Glbal varibles 
 */ 
ESP8266WebServer http_rest_server(HTTP_REST_PORT);

DHT_Unified dht(DHTPIN, DHTTYPE);
Config config;

RtcDS3231<TwoWire> rtc(Wire);

// This is to calculate the time elapsed
unsigned long lastTimeUploadData = 0;

void setup() {
  Serial.println('Starting ...');
  // put your setup code here, to run once:
  loadDefaultConfig();
  Serial.begin(115200);

  setupConfigFile();
  
  setupRtcModule();

  // enable Temperature sensor
  dht.begin();

  // Setup pin for LEDs
  pinMode(WIFI_LED_PIN, OUTPUT);
  digitalWrite(WIFI_LED_PIN, HIGH);

  setupEsp8266Client();
  setupesp8266AccessPoint();
}

void loop() {
  http_rest_server.handleClient();
  
  // polling interval
  unsigned long now = millis();
  unsigned long intervalSinceLastSend = now - lastTimeUploadData;

//   upload temperature data to server
  if(intervalSinceLastSend >= config.pollingInterval) {
    if (WiFi.status() == WL_CONNECTED) {
      if (isActivated() == true) {
        uploadData();
      }
    }
//    Serial.println(intervalSinceLastSend);
//    Serial.println(config.pollingInterval);
    lastTimeUploadData = now;
  }

  if (WiFi.status() != WL_CONNECTED) {
    doubleBlink(100);
  } else {
    blink(100);
  }

  // delay 1s per loop
  delay(1000);
}

void setupRtcModule() {
  Serial.println("Setup RTC DS3231");
  rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  if (!rtc.IsDateTimeValid()) 
  {
      if (rtc.LastError() != 0)
      {
          // we have a communications error
          // see https://www.arduino.cc/en/Reference/WireEndTransmission for 
          // what the number means
          Serial.print("RTC communications error = ");
          Serial.println(rtc.LastError());
      } else {
        Serial.println("RTC lost confidence in the DateTime!");
        rtc.SetDateTime(compiled);
      }

      if (!rtc.GetIsRunning())
      {
          Serial.println("RTC was not actively running, starting now");
          rtc.SetIsRunning(true);
      }
  }

  RtcDateTime now = rtc.GetDateTime();
  if (now < compiled) 
  {
      Serial.println("RTC is older than compile time!  (Updating DateTime)");
      rtc.SetDateTime(compiled);
  }
  else if (now > compiled) 
  {
      Serial.println("RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled) 
  {
      Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }

  rtc.Enable32kHzPin(false);
  rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
}
