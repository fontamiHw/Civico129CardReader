#ifndef CARDREADERMQTT_H
#define CARDREADERMQTT_H

#pragma once

#include <ArduinoMqttClient.h>
#include <WiFi.h>

typedef struct MqttBrokerParam_t
{
    char *broker;
    int port;
    char *mqttUser;
    char *mqttPass;
} MqttBrokerParam;

typedef struct MqttMessage_t
{
    char *topic;
    char *payload;
} MqttMessage;

typedef struct WifiParam_t
{
    char *ssid;
    char *pwd;
} WifiParam;

class CardReaderMqtt
{
public:
    CardReaderMqtt(const char *ssid, const char *pwd);
    ~CardReaderMqtt();
    void addMqttParam(const char *broker, int port, const char *mqttUser, const char *mqttPass);
    bool connect();
    void poll();
    void sendMessage(const char *topic, const char *payload);
    void subscribe(const char *topic);
    void handleSubscription();


private:
    CardReaderMqtt(); // not allowed empty constructor
    WifiParam wifiParam;
    MqttBrokerParam mqttBrokerParam;
    // To connect with SSL/TLS:
    // 1) Change WiFiClient to WiFiSSLClient.
    // 2) Change port value from 1883 to 8883.
    // 3) Change broker value to a server with a known SSL/TLS root certificate
    //    flashed in the WiFi module.
    MqttClient *mqttClient;
    bool connectToWifi();
    bool connectToBroker();
};

#endif