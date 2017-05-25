#ifndef _GreenhouseIrrigation_
#define _GreenhouseIrrigation_
#include"config.h"

/*
#define D0 16
#define D1 5 // I2C Bus SCL (clock)
#define D2 4 // I2C Bus SDA (data)
#define D3 0
#define D4 2 // Same as "LED_BUILTIN", but inverted logic
#define D5 14 // SPI Bus SCK (clock)
#define D6 12 // SPI Bus MISO 
#define D7 13 // SPI Bus MOSI
#define D8 15 // SPI Bus SS (CS)
#define D9 3 // RX0 (Serial console)
#define D10 1 // TX0 (Serial console)
*/

typedef struct {
  char waterPumpError;
} errorType;
class GreenhouseIrrigation {
  public:
    GreenhouseIrrigation();
    ~GreenhouseIrrigation();
    void run();
    void startIrrigation();
    void stopIrrigation();
    void setMaxLitersOfWater(unsigned int  liters);

    unsigned int getWaterMeterValue();
    char isWaterPumpRunning();

    errorType *isErrors();

    static void (*infoCallback)(const char*);
    static void (*motorCallback)(char);

  private:
    unsigned int maxLitersOfWater;
    unsigned int  watermeter;
    unsigned int  wantedLitersOfWater;
    unsigned long startTime;
    errorType errors;

    char initialized;
};
#endif

