#ifndef MQTTPUBSUB_H_
#define MQTTPUBSUB_H_

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "status.h"
#include "./MqttMessageHandler.h"

class MqttPubSub
{
public:
  PubSubClient client;
  int lastMillis = 0;
  MqttPubSub();
  void publishStatus(bool waitForInterval);
  void setup();
  void handle();
  void sendMessage(String message);
  void sendMesssageToTopic(const char *topic, const char *message);
  void sendMessage(String message, String channel);
  void sendMesssageToTopic(const char *topic, String message);

private:
  char tempBuffer[1024];
  long lastReconnectAttempt = -10000;
  char channelStatus[50];
  char channelIn[50];
  
  const char *hostname;
  WiFiConfig currentMqttConfig;
  WiFiClient espClient;
  bool connect(const char *id, const char *username, const char *password);
  bool reconnect();
  static void callback(char *topic, byte *message, unsigned int length);
};

#endif /* MQTTPUBSUB_H_ */
