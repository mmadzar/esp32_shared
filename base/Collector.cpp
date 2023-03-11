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
    collectedSamples++;
    this->timestamp = timestamp;
    this->value = this->value + value;
    if (value > max || collectedSamples == 1)
        max = value;
    if (value < min || collectedSamples == 1)
        min = value;
    handle();
}

void Collector::handle()
{
    if (status.currentMillis - lastSend > config->sendRate)
    {
        lastSend = status.currentMillis;
        double lastAverage1 = ((double)this->value / (double)(collectedSamples == 0 ? 1 : collectedSamples));
        if (lastAverage1 < 0.0)
            lastAverage1 -= 0.5;
        else
            lastAverage1 += 0.5;
        lastAverage = (int)lastAverage1;
        _change_callback(config->name, lastAverage, min, max, collectedSamples, timestamp);
        // reset collector
        collectedSamples = 0;
        this->value = 0;
    }
}
