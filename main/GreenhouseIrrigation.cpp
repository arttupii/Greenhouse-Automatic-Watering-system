#include"GreenhouseIrrigation.h"
#include<Arduino.h>
#include"config.h"


void (*GreenhouseIrrigation::motorCallback)(char);
void (*GreenhouseIrrigation::infoCallback)(const char*);

void waterPump(char onOff) {
  Serial.printf("WaterPump is %s\n", onOff ? "on" : "off");
  if (onOff) {
    digitalWrite(PUMP_PIN, LOW);
  } else {
    digitalWrite(PUMP_PIN, HIGH);
  }

  GreenhouseIrrigation::motorCallback(onOff);
}

//Flow rate pulse characteristics: Frequency (Hz) = 7.5 * Flow rate (L/min)
unsigned int pulsesFromFlowSensor = 0;
void waterMeterPuls() {
  pulsesFromFlowSensor++;
}

GreenhouseIrrigation::GreenhouseIrrigation() {
  maxLitersOfWater = 0;
  watermeter = 0;
  wantedLitersOfWater = 0;
  errors = false;
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  pinMode(PUMP_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), waterMeterPuls, FALLING);

  initialized = 0;
}

GreenhouseIrrigation::~GreenhouseIrrigation() {

}

unsigned int GreenhouseIrrigation::getWaterMeterValue() {
  return watermeter;
}

char GreenhouseIrrigation::isWaterPumpRunning() {
  return wantedLitersOfWater > 0;
}

bool GreenhouseIrrigation::isErrors() {

  return errors;
}

bool change(unsigned int x, unsigned int y, unsigned int c) {
  unsigned int cc = 0;
  if (x > y) {
    cc = x - y;
  } else {
    cc = y - x;
  }
  return cc < c;
}


void GreenhouseIrrigation::run() {
  if (initialized) {
    initialized = 1;
    waterPump(0);
  }

  if (wantedLitersOfWater > 0) {
    if (watermeter >= wantedLitersOfWater) {
      waterPump(0);
      wantedLitersOfWater = 0;
      infoCallback("Pump off");
    } else {
      if (millis() > startTime + MAX_WATERPUMP_RUNNING_TIME) {
        if (change(last_watermeter, watermeter, 20))  {
          Serial.println("Irrigation failure");
          errors = true;
          infoCallback("Pump error");
          wantedLitersOfWater = 0;
          waterPump(0);
        }
        startTime = millis();
        last_watermeter = watermeter;
      }
    }
    watermeter++;
  }

  if (pulsesFromFlowSensor > 0) {
    watermeter = (pulsesFromFlowSensor * 10) / LITERS_PER_PULSE; // 450; (450 pulses/l --> 45 pulses / dl)
  } else {
    watermeter = 0;
  }
}

void GreenhouseIrrigation::startIrrigation() {
  if (maxLitersOfWater <= 0) {
      Serial.println("Do not start water pump (maxLitersOfWater==0)");
    return;
  }
  Serial.println("Start water pump");

  last_watermeter = watermeter;
  wantedLitersOfWater = watermeter + maxLitersOfWater;
  startTime = millis();
  static char tmp[30];
  sprintf(tmp, "Pump on [%d l]", maxLitersOfWater / 10);
  waterPump(1);
  errors = 0;
  infoCallback(tmp);
}

void GreenhouseIrrigation::resetWaterMeter() {
  maxLitersOfWater = 0;
  watermeter = 0;
  wantedLitersOfWater = 0;
  errors = false;
}
void GreenhouseIrrigation::stopIrrigation() {
  Serial.println("Stop water pump");
  wantedLitersOfWater = 0;
  waterPump(0);
  infoCallback("Stop pump");
}
void GreenhouseIrrigation::setMaxLitersOfWater(unsigned int  liters) {
  Serial.print("Set maxLitersOfWater = ");
  Serial.println(liters);
  maxLitersOfWater = liters * 10;
}

