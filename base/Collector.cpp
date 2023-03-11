#include "Collector.h"
#include "status.h"

Collector::Collector(CollectorConfig &Collectorconfig)
{
    config = &Collectorconfig;
}

void Collector::setup()
{
}

int &Collector::onChange(THandlerFunction_Change fn)
{
    _change_callback = fn;
    return lastAverage;
}

void Collector::handle(int value, uint64_t timestamp)
{
    this->timestamp=timestamp;
    collectedSamples++;
    if (value > max || collectedSamples == 1)
        max = value;
    if (value < min || collectedSamples == 1)
        min = value;
    if (collectedSamples == 1)
        this->value = 0;
    this->value = this->value + value;
    handle();
}

void Collector::handle()
{
    if (status.currentMillis - lastSend > config->sendRate)
    {
        lastSend = status.currentMillis;
        lastAverage = (int)((double)this->value / (double)(collectedSamples == 0 ? 1 : collectedSamples));
        _change_callback(config->name, lastAverage, min, max, collectedSamples, timestamp);
        collectedSamples = 0;
        //reset value on counter in other handle() to avoid reseting to 0 when no samples received this->value = 0;
    }
}
