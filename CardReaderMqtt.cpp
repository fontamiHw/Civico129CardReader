#include <Arduino.h>

#include "CardReaderMqtt.h"

WiFiClient wifiClient;

CardReaderMqtt::CardReaderMqtt()
{
}

CardReaderMqtt::~CardReaderMqtt()
{
}

CardReaderMqtt::CardReaderMqtt(const char *ssid, const char *pwd)
{
    this->wifiParam.ssid = new char[strlen(ssid) + 1];
    strcpy(this->wifiParam.ssid, ssid);

    this->wifiParam.pwd = new char[strlen(pwd) + 1];
    strcpy(this->wifiParam.pwd, pwd);
}

void CardReaderMqtt::addMqttParam(const char *broker, int port, const char *mqttUser, const char *mqttPass)
{
    this->mqttBrokerParam.broker = new char[strlen(broker) + 1];
    strcpy(this->mqttBrokerParam.broker, broker);

    this->mqttBrokerParam.port = port;

    this->mqttBrokerParam.mqttUser = new char[strlen(mqttUser) + 1];
    strcpy(this->mqttBrokerParam.mqttUser, mqttUser);

    this->mqttBrokerParam.mqttPass = new char[strlen(mqttPass) + 1];
    strcpy(this->mqttBrokerParam.mqttPass, mqttPass);
}

bool CardReaderMqtt::connect()
{
    if (connectToWifi())
    {
        return connectToBroker();
    }
    else
    {
        return false;
    }
}

void CardReaderMqtt::poll()
{
    this->mqttClient->poll();
}

void CardReaderMqtt::sendMessage(const char *topic, const char *payload)
{
    Serial.print("Sending msg: ");
    Serial.print(payload);    
    Serial.print(" to topic: ");
    Serial.println(topic);
    this->mqttClient->beginMessage(topic);
    this->mqttClient->print(payload);
    this->mqttClient->endMessage();
    Serial.println("Message sent!");
}
bool CardReaderMqtt::connectToWifi()
{
    bool connected = false;
    this->mqttClient = new MqttClient(wifiClient);

    // attempt to connect to WiFi network:
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(this->wifiParam.ssid);
    Serial.print("and password: ");
    Serial.println(this->wifiParam.pwd);

    WiFi.begin(this->wifiParam.ssid, this->wifiParam.pwd);
    // Auto reconnect is set true as default
    // To set auto connect off, use the following function
    //    WiFi.setAutoReconnect(false);
    // Will try for about 10 seconds (20x 500ms)
    int tryDelay = 500;
    int numberOfTries = 20;

    // Wait for the WiFi event
    while (!connected)
    {
        switch (WiFi.status())
        {
        case WL_NO_SSID_AVAIL:
            Serial.println("[WiFi] SSID not found");
            break;
        case WL_CONNECT_FAILED:
            Serial.print("[WiFi] Failed - WiFi not connected! Reason: ");
            return false;
            break;
        case WL_CONNECTION_LOST:
            Serial.println("[WiFi] Connection was lost");
            break;
        case WL_SCAN_COMPLETED:
            Serial.println("[WiFi] Scan is completed");
            break;
        case WL_DISCONNECTED:
            Serial.println("[WiFi] WiFi is disconnected");
            break;
        case WL_CONNECTED:
            Serial.println("[WiFi] WiFi is connected!");
            Serial.print("[WiFi] IP address: ");
            Serial.println(WiFi.localIP());
            connected = true;
            break;
        default:
            Serial.print("[WiFi] WiFi Status: ");
            Serial.println(WiFi.status());
            break;
        }
        delay(tryDelay);

        if (numberOfTries <= 0)
        {
            Serial.print("[WiFi] Failed to connect to WiFi!");
            // Use disconnect function to force stop trying to connect
            WiFi.disconnect();
            return false;
        }
        else
        {
            numberOfTries--;
        }
    }

    Serial.println("You're connected to the network");
    Serial.println();

    return true;
}

bool CardReaderMqtt::connectToBroker()
{
    // You can provide a unique client ID, if not set the library uses Arduino-millis()
    // Each client must have a unique client ID
    mqttClient->setId("NfcReaderClient");

    // You can provide a username and password for authentication
    mqttClient->setUsernamePassword(mqttBrokerParam.mqttUser, mqttBrokerParam.mqttPass);

    Serial.print("Attempting to connect to the MQTT broker: ");
    Serial.println(mqttBrokerParam.broker);

    if (!mqttClient->connect(mqttBrokerParam.broker, mqttBrokerParam.port))
    {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient->connectError());

        while (1);
    }

    Serial.println("You're connected to the MQTT broker!");
    Serial.println();
    return true;
}