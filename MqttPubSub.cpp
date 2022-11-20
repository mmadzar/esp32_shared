#include "MqttPubSub.h"

MqttMessageHandler mqttMessageHandler;

MqttPubSub::MqttPubSub()
{
}

void MqttPubSub::setup()
{
  hostname = wifiSettings.hostname;
  sprintf(channelStatus, "%s/status", hostname);
  sprintf(channelIn, "%s/in/#", hostname);

  client.setClient(espClient);
  client.setBufferSize(1024);
  client.setKeepAlive(30);
  client.setCallback(callback);

  SETTINGS.loadSettings();
}

void MqttPubSub::callback(char *topic, byte *message, unsigned int length)
{
  char msg[length + 1];
  for (size_t i = 0; i < length; i++)
    msg[i] = (char)message[i];
  msg[length] = 0x0a; // important to add termination to string! messes string value if ommited

  String t = String(topic);
  String cmd = t.substring(String(wifiSettings.hostname).length() + 4, t.length());
  if (length > 0)
  {
    if (cmd == "restart" && String(msg).toInt() == 1)
      ESP.restart();
    else if (cmd == "reconnect" && String(msg).toInt() == 1)
      WiFi.disconnect(false, false);
    else
    {
      mqttMessageHandler.HandleMessage(cmd.c_str(), msg, length + 1);
      mqttMessageHandler.callback(topic, message, length);
    }

    // status.receivedCount++;
  }
}

bool MqttPubSub::reconnect()
{
  if (strcmp(status.SSID, "") != 0)
  {
    if (strcmp(status.SSID, currentMqttConfig.ssid) != 0)
    {
      // reset current MQTT configuration
      for (int i = 0; i < AP_COUNT; i++)
      {
        if (strcmp(status.SSID, SETTINGS.APlist[i].ssid) == 0)
        {
          memcpy((void *)&currentMqttConfig, (void *)&SETTINGS.APlist[i], sizeof(currentMqttConfig));
          if (strcmp(currentMqttConfig.mqtt.server, "gateway") == 0)
            currentMqttConfig.mqtt.server = strdup(status.gatewayAddress);
          break;
        }
      }
    }

    if (currentMqttConfig.ssid != "")
    {
      Serial.println("Connecting to MQTT...");
      // Attempt to connect
      client.setServer(currentMqttConfig.mqtt.server, currentMqttConfig.mqtt.port);
      if (connect(HOST_NAME, currentMqttConfig.mqtt.username, currentMqttConfig.mqtt.password))
      {
        client.subscribe(channelIn);
        Serial.print("Listening: ");
        Serial.println(channelIn);

        // subscribe to extra channels
        // client.subscribe("#");

        digitalWrite(pinsSettings.led, LOW);
        publishStatus(false);
      }
    }
    return client.connected();
  }
  return false;
}

bool MqttPubSub::connect(const char *id, const char *username, const char *password)
{
  if (username == "")
  {
    return client.connect(id);
  }
  else
  {
    return client.connect(id, username, password);
  }
}

void MqttPubSub::publishStatus(bool waitForInterval) // TODO pass additional status values
{
  if (!waitForInterval || ((millis() - lastMillis) > intervals.statusPublish))
  {
    lastMillis = millis();
    serializeJson(status.GenerateJson(), tempBuffer);
    client.publish(channelStatus, tempBuffer);
    status.missedSend = 0;
  }
  else
  {
    status.missedSend++;
  }
}

void MqttPubSub::handle()
{
  if (strcmp(status.SSID, "") == 0)
    lastReconnectAttempt = -10000; // reconnects as soon as connected to WiFi

  if (strcmp(status.SSID, "") != 0)
  {
    if (!client.connected())
    {
      digitalWrite(2, HIGH);                                                                                                                                                                                                                                                              // set notification led
      if (!(status.currentMillis - lastReconnectAttempt < 200 || (status.currentMillis - lastReconnectAttempt > 500 && status.currentMillis - lastReconnectAttempt < 700) || (status.currentMillis - lastReconnectAttempt > 1000 && status.currentMillis - lastReconnectAttempt < 1200))) // reset notification led
        digitalWrite(2, LOW);
      if (status.currentMillis - lastReconnectAttempt > 10000) // reconnect to MQTT every 10 seconds
      {
        // Serial.println("Mqtt - Client not connected!");
        lastReconnectAttempt = status.currentMillis;
        // Attempt to reconnect
        if (reconnect())
        {
          lastReconnectAttempt = -10000;
          Serial.println("mqtt connected."); // Expired waiting on inactive connection...");
        }
      }
    }
    else
    {
      // Client connected
      client.loop();
    }
  }

  mqttMessageHandler.handle();
}

void MqttPubSub::sendMessage(String message, String channel)
{
  char msg[255];
  message.toCharArray(msg, 255);
  char chnl[255];
  channel.toCharArray(chnl, 255);

  client.publish(chnl, msg);
}

void MqttPubSub::sendMesssageToTopic(const char *topic, String message)
{
  char msg[255];
  message.toCharArray(msg, 255);
  client.publish(topic, msg);
}

void MqttPubSub::sendMesssageToTopic(const char *topic, const char *message)
{
  client.publish(topic, message);
}
