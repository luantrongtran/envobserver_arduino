//#include <Wire.h>
//#include <RtcDS3231.h>
//#include <ArduinoJson.h>

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

void tripleBlink(int interval) {
  blink(interval);
  delay(interval);
  blink(interval);
  delay(interval);
  blink(interval);
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
