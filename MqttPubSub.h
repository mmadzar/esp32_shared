#ifndef MQTTPUBSUB_H_
#define MQTTPUBSUB_H_

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "appconfig.h"
#include "../../secrets.h"
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
  void sendMessageToTopic(const char *topic, const char *message);
  void sendMessageToTopic(const char *topic, String message);
  void sendMessageToTopic(String topic, const char *message);
  void sendMessageToTopic(String message, String topic);

private:
  char tempBuffer[2048];
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
