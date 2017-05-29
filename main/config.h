#ifndef _config_
#define _config_


#define DHTPIN            D5         // Pin which is connected to the DHT sensor.

// Uncomment the type of sensor in use:
#define DHTTYPE           DHT11     // DHT 11 
//#define DHTTYPE           DHT22     // DHT 22 (AM2302)
//#define DHTTYPE           DHT21     // DHT 21 (AM2301)


#define MAX_WATERPUMP_RUNNING_TIME 10000
#define LITERS_PER_PULSE 450
#define FLOW_SENSOR_PIN D1
#define PUMP_PIN D0
#define BLYNK_AUTH "xxxxxxxxxxxxxx"
#define WIFI_SSID "xxxxxxxxxxxxxxxx"
#define WIFI_PASS "xxxxxxxxxxxxxxxxxx"


#define BLYNK_PUMP_SWITCH V0
#define BLYNK_AMOUNT_OF_WATER V1
#define BLYNK_LCD V2
#define BLYNK_TIMER_1 V3
#define BLYNK_TIMER_2 V4
#define BLYNK_ERROR V5
#define BLYNK_RESET V6
#define BLYNK_TEMP V7
#define BLYNK_HUMIDITY V8

#endif
