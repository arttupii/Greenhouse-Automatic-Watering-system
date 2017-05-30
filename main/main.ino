#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include<Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "config.h"
#include <DHT.h>

#ifdef OTA
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif

WidgetLCD lcd(BLYNK_LCD);

#include "GreenhouseIrrigation.h"

DHT dht(DHTPIN, DHTTYPE);

GreenhouseIrrigation ghi;

char auth[] = BLYNK_AUTH;

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

WidgetLED errorLed(BLYNK_ERROR);
void lcdUpdate(bool force = false) ;

class MessageBuffer {
  public:
    MessageBuffer() {
      clear();
      t = millis();
      c = 0;
    };
    ~MessageBuffer() {};
    void add(const char* msg) {
      int bl = strlen(buffer);
      int ml = strlen(msg);
      if (bl + ml > sizeof(buffer)) {
        return;
      }
      memcpy(&buffer[bl], msg, ml);
      newTexts = true;
    }
    void clear() {
      memset(buffer, 0, sizeof(buffer));
      sprintf(buffer, "%d-->", c);
      c++;
      newTexts = false;
    }
    void run() {
      int bl = strlen(buffer);
      if (bl > 0 && newTexts) {
        if (t + 20000 < millis()) {
          Blynk.tweet(buffer);
          clear();
          t = millis();
        }
      }
    }
  private:
    char buffer[140];
    unsigned long int t;
    int c;
    bool newTexts;
};

MessageBuffer bf;

void updateInfo(const char*text) {
  if (Blynk.connected()) {
    lcd.print(0, 1, "                         ");
    lcd.print(0, 1, text);

    bf.add(text);
    bf.add("\n");
    if (ghi.isErrors()) //LED 10 "HIGH"
      errorLed.on();
    else
      errorLed.off();
  }
}


void motorUpdate(char onOff) {
  if (Blynk.connected()) {
    Blynk.virtualWrite(BLYNK_PUMP_SWITCH, onOff);
  }
}

void setup()
{
  GreenhouseIrrigation::infoCallback = updateInfo;
  GreenhouseIrrigation::motorCallback = motorUpdate;

  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  // Setup a function to be called every second
  //sensors.begin();

  Serial.printf("Flow sensor pin = %d\n", FLOW_SENSOR_PIN);
  Serial.printf("Pump sensor pin = %d\n", PUMP_PIN);

  dht.begin();
#ifdef OTA
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.setPassword((const char *)OTA_PASSWORD);
  ArduinoOTA.setHostname(OTA_HOSTNAME);

  ArduinoOTA.begin();

#endif
}

BLYNK_CONNECTED() {
  Blynk.syncAll();
}

BLYNK_WRITE(BLYNK_PUMP_SWITCH)
{
  int value = param.asInt();
  if (value) {
    ghi.startIrrigation();
  } else {
    ghi.stopIrrigation();
  }
}

BLYNK_WRITE(BLYNK_RESET)
{
  int value = param.asInt();
  if (value) {
    ghi.resetWaterMeter();
    lcdUpdate(true);
  }
}

BLYNK_READ(BLYNK_PUMP_SWITCH)
{
  Blynk.virtualWrite(BLYNK_PUMP_SWITCH, ghi.isWaterPumpRunning());  //LED 10 "HIGH"
}
BLYNK_READ(BLYNK_TIMER_1)
{
  Blynk.virtualWrite(BLYNK_PUMP_SWITCH, ghi.isWaterPumpRunning());  //LED 10 "HIGH"
}
BLYNK_READ(BLYNK_TIMER_2)
{
  Blynk.virtualWrite(BLYNK_PUMP_SWITCH, ghi.isWaterPumpRunning());  //LED 10 "HIGH"
}
BLYNK_READ(BLYNK_ERROR)
{
  if (ghi.isErrors()) //LED 10 "HIGH"
    errorLed.on();
  else
    errorLed.off();
}


BLYNK_WRITE(BLYNK_AMOUNT_OF_WATER)
{
  float value = param.asInt();
  ghi.setMaxLitersOfWater(value);
}

const char* temperatureAsSrt() {
  static char tmp[10];
  int t = (int)(dht.readTemperature(false, true));
  if (t > 50 || t < -50) {
    return "--C";
  }
  sprintf(tmp, "% 3dC", t);
  return tmp;
}

const char*humityAsSrt() {
  static char tmp[10];
  int dd = (int)dht.readHumidity(true);
  if (dd > 100 || dd < 0) {
    return "--%";
  }
  sprintf(tmp, "% 2d\%", dd);
  return tmp;
}

const char*waterMeterValueAsSrt() {
  static char tmp[10];
  int dd = ghi.getWaterMeterValue();
  sprintf(tmp, "%05d.%01d", dd / 10, dd % 10);
  return tmp;
}

void lcdUpdate(bool force) {
  static int long t = millis();
  if (millis() > t || force)  {
    t = millis() + 5000;
    lcd.print(0, 0, temperatureAsSrt());
    lcd.print(4, 0, humityAsSrt());
    lcd.print(9, 0, waterMeterValueAsSrt());
  }
}

void updateTempAndHum() {
  static int long t = millis();
  if (t < millis()) {
    t = millis() + 10000;
    Blynk.virtualWrite(BLYNK_TEMP, dht.readTemperature(false, true));
    Blynk.virtualWrite(BLYNK_HUMIDITY, dht.readHumidity(true));

  }
}
void loop()
{
  Blynk.run();
  ghi.run();
  lcdUpdate();
  bf.run();
  updateTempAndHum();
#ifdef OTA
  ArduinoOTA.handle();
#endif
}


