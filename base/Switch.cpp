#include "Switch.h"

Switch::Switch(int index, SwitchConfig &switchconfig)
{
    config = &switchconfig;
}

void Switch::setup()
{
    switch (config->switchtype)
    {
    case switcht::on_off:
    case switcht::click_once:
        pinMode(config->pin, OUTPUT);
        set(0);
        break;
    case switcht::pwm_signal:

        // Initialize channels
        // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
        if (config->channel == 3)
            ledcSetup(config->channel, 1000, 8); // 1 kHz PWM, 8-bit resolution (0-255) - for charger EVSE
        else
            ledcSetup(config->channel, 200, 7); // 200 Hz PWM, 7-bit resolution (0-127) - for pump
        set(0);
        ledcAttachPin(config->pin, config->channel); // assign a pin to a channel
        break;
    default:
        break;
    }
    digitalWrite(config->pin, lastValueSet);
}

int &Switch::onChange(THandlerFunction_Change fn)
{
    _change_callback = fn;
    return lastValueSet;
}

void Switch::set(int value)
{
    status.switches[settings.getSwitchIndex(config->device)] = value;
}

void Switch::handle()
{
    int ix = settings.getSwitchIndex(config->device);

    // change click_once button state
    if (lastTimeSet != -1 && config->switchtype == switcht::click_once && status.currentMillis - lastTimeSet > intervals.click_onceDelay)
    {
        status.switches[ix] = abs(status.switches[ix] - 1);
        lastTimeSet = -1;
    }

    if (status.switches[ix] != lastValueSet)
    {
        lastValueSet = status.switches[ix];
        switch (config->switchtype)
        {
        case switcht::on_off:
            digitalWrite(config->pin, lastValueSet);
            break;
        case switcht::pwm_signal:
            ledcWrite(config->channel, lastValueSet); // set the speed of pump - uses inverted value because GND is switched
            break;
        case switcht::click_once:
            digitalWrite(config->pin, lastValueSet);
            lastTimeSet = status.currentMillis;
            break;
        default:
            break;
        }

        _change_callback(config->name, config->device, lastValueSet);
    }

    // // write pwm every time
    // if (config->switchtype == switcht::pwm_signal)
    // {
    //     ledcWrite(config->channel, lastValueSet); // set the speed of pump - write inverted value
    // }
}