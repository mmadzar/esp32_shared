#ifndef SWITCH_H_
#define SWITCH_H_

#include <Arduino.h>
#include "../src/appconfig.h"
#include "../src/status.h"

class Switch
{
public:
    // callback event related
    typedef void (*THandlerFunction_Change)(const char *name, devicet devicetype, int value);
    int &onChange(THandlerFunction_Change fn); // This callback will be called when reading changes or timeout is exceeded

    Switch(int index, SwitchConfig &config);
    void setup();
    void set(int value);
    void handle();

private:
    SwitchConfig *config;
    int lastValueSet = 0; // default value
    int lastTimeSet = -1; // milliseconds
    THandlerFunction_Change _change_callback;
};

#endif