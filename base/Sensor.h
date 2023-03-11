#ifndef SENSOR_H_
#define SENSOR_H_

#include <Arduino.h>
#include "../src/appconfig.h"
#include "../src/status.h"

class Sensor
{
public:
    // callback event related
    typedef void (*THandlerFunction_Change)(const char *name, devicet devicetype, int value);
    int &onChange(THandlerFunction_Change fn); // This callback will be called when reading changes or timeout is exceeded

    Sensor(SensorConfig &config);
    void setup();
    void handle();

private:
    SensorConfig *config;
    int lastValueRead = 0; // last averaged adc value read from pin
    THandlerFunction_Change _change_callback;

    double To = 298.15; // temperature only - temperature in Kelvin for 25 degree Celsius
    double Beta = 3950; // temperature only - Beta value of thermistor

    const double A = 0.001129148; // thermistor equation parameters
    const double B = 0.000234125;
    const double C = 0.0000000876741;

    double calculateTemperature(int adc_value);

    double calculateVoltage(int adc_value);
    long tempValues;
    const int sumValuesCount = 10; // number of samples to collect before sending average 1k = 11687 missedIntervals, 50 = 13583 missedIntervals
    int minValue = 5000;           // minimum value read in samples collected - expected highest value 4096
    int maxValue = 0;              // maximum value read in samples collected
    int samplesCollected = 0;
    const int skipSteps = 80; // skip steps between pin value reads
    int skipStepsCounter = 0;

    long lastOnChangeTime = 0; // milliseconds when last value change was published
    int millisecondsBetweenOnChanges = 250;
};

#endif