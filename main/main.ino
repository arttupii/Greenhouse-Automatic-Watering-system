#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include<Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <BlynkSimpleEsp8266.h>
#include "config.h"

WidgetLCD lcd(V2);

#include <OneWire.h>
#include <DallasTemperature.h>
#include "GreenhouseIrrigation.h"


GreenhouseIrrigation ghi;

// Setup a oneWire D4 to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(D4);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress insideThermometer;

char auth[] = BLYNK_AUTH;

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;


void updateInfo(const char*text) {
  if(Blynk.connected()) {
    lcd.print(0, 1, "                         ");
    lcd.print(0, 1, text);
    Blynk.tweet(text);
  }
}


void motorUpdate(char onOff) {
    if(Blynk.connected()) {
      Blynk.virtualWrite(V0, onOff);
    }
}

void setup()
{
  GreenhouseIrrigation::infoCallback = updateInfo;
  GreenhouseIrrigation::motorCallback = motorUpdate;

  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  // Setup a function to be called every second
  sensors.begin();

}

/*
  V0 Pummppu
  V1 Kastelu määrä
  V2 LCD
  V3 timer1
  V4 timer2
*/

void updateLcd() {
  lcd.print(0, 0, "ULTIMATE");
}

BLYNK_CONNECTED() {
  Blynk.syncAll();
}

BLYNK_WRITE(V0)
{
  int value = param.asInt();
  if (value) {
    ghi.startIrrigation();
  } else {
    ghi.stopIrrigation();
  }
}

BLYNK_READ(V0)
{
      Blynk.virtualWrite(V0, ghi.isWaterPumpRunning());  //LED 10 "HIGH"
}

BLYNK_WRITE(V1)
{
  float value = param.asInt();
  ghi.setMaxLitersOfWater(value);
}

void lcdUpdate() {
  static int long t = millis();
  if (millis() > t)  {
    t = millis() + 5000;
    lcd.print(0, 0, "24.6C");
    lcd.print(6, 0, "99%");
    lcd.print(11, 0, ghi.getWaterMeterValue()/10);
  }
}

void loop()
{
  Blynk.run();
  ghi.run();
  lcdUpdate();
}


