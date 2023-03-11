#ifndef COLLECTOR_H_
#define COLLECTOR_H_

#include <Arduino.h>
#include "shared/configtypes/configtypes.h"
#include "appconfig.h"

class Collector
{
public:
    // callback event related
    typedef void (*THandlerFunction_Change)(const char *name, int value, int min, int max, int collectedSamples, uint64_t timestamp);
    int &onChange(THandlerFunction_Change fn); // This callback will be called when reading changes or timeout is exceeded

    Collector(CollectorConfig &config);
    void setup();
    void handle();
    void handle(int value, uint64_t timestamp);

private:
    CollectorConfig *config;
    long lastSend = 0; // last miliseconds when message was send
    THandlerFunction_Change _change_callback;

    long value = 0;           // accumulated value
    int collectedSamples = 0; // number of collected samples
    int lastAverage = 0;      // last calculated average
    int min = 0;
    int max = 0;
    uint64_t timestamp = 0; // UTC microseconds timestamp
};

#endif