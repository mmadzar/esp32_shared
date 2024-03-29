#ifndef STATUS_BASE_H_
#define STATUS_BASE_H_

#include <stdint.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>

class StatusBase
{
public:
    struct tm timeinfo;

    char upsince[29];
    char connectedsince[29];
    long connectCount = 0;
    long loops = 0;
    long missedSend = 0;
    long currentMillis = 0;
    uint32_t freeMem = 0;
    uint32_t minFreeMem = 0;
    const char *ipAddress = "255.255.255.255";
    const char *gatewayAddress = "255.255.255.255";
    const char *SSID = "                  ";
    int8_t rssi = 0;
    long receivedCount = 0;
    StaticJsonDocument<1024> doc;

    JsonObject PrepareRoot()
    {
        JsonObject root = doc.to<JsonObject>();
        // refresh values
        rssi = WiFi.RSSI();
        freeMem = esp_get_free_heap_size();
        minFreeMem = esp_get_minimum_free_heap_size();
        root["uptime"] = (currentMillis / 600) / 100.00;
        root["loops"] = loops;
        root["upSince"] = upsince;
        root["connectedSince"] = connectedsince;
        root["connectCount"] = connectCount;
        root["freeMem"] = freeMem;
        root["minFreeMem"] = minFreeMem;
        root["ipAddress"] = ipAddress;
        root["gateway"] = gatewayAddress;
        root["location"] = "car";
        root["SSID"] = SSID;
        root["RSSI"] = rssi;
        root["receivedCount"] = receivedCount;

        return root;
    }

    void getTimestamp(char *buffer)
    {
        if (strcmp(SSID, "") == 0 || !getLocalTime(&(timeinfo), 10))
            sprintf(buffer, "INVALID TIME               ");
        else
        {
            long microsec = 0;
            struct timeval tv;
            gettimeofday(&tv, NULL);

            microsec = tv.tv_usec;
            strftime(buffer, 29, "%Y-%m-%d %H:%M:%S", &(timeinfo));
            sprintf(buffer, "%s.%06d", buffer, microsec);
        }
    }

    uint64_t getTimestampMicro()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (tv.tv_sec * 1000.0 + (tv.tv_usec / 1000.0));
    }
};

#endif /* STATUS_BASE_H_ */
