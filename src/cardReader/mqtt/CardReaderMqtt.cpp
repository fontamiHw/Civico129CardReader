#include <Arduino.h>

#include "CardReaderMqtt.h"
#include "src/global.h"

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
#ifdef PRINT
    Serial.print("Sending msg: ");
    Serial.print(payload);
    Serial.print(" to topic: ");
    Serial.println(topic);
#endif

    this->mqttClient->beginMessage(topic);
    this->mqttClient->print(payload);
    this->mqttClient->endMessage();

#ifdef PRINT
    Serial.println("Message sent!");
#endif
}

void CardReaderMqtt::subscribe(const char *topic)
{
#ifdef PRINT
    Serial.print("Subscribing to topic: ");
    Serial.println(topic);
#endif
    this->mqttClient->subscribe(topic);
}
void CardReaderMqtt::handleSubscription()
{
    int messageSize = this->mqttClient->parseMessage();
    if (messageSize)
    {
#ifdef PRINT
        // we received a message, print out the topic and contents
        Serial.print("Received a message with topic '");
        Serial.print(this->mqttClient->messageTopic());
        Serial.print("', length ");
        Serial.print(messageSize);
        Serial.println(" bytes:");
#endif
        // use the Stream interface to print the contents
        while (this->mqttClient->available())
        {
#ifdef PRINT
            Serial.print((char)this->mqttClient->read());
#endif
        }
#ifdef PRINT
        Serial.println();
#endif
    }
}

bool CardReaderMqtt::connectToWifi()
{
    bool connected = false;
    this->mqttClient = new MqttClient(wifiClient);

#ifdef PRINT
    // attempt to connect to WiFi network:
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(this->wifiParam.ssid);
    Serial.print("and password: ");
    Serial.println(this->wifiParam.pwd);
#endif

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
#ifdef PRINT
            Serial.println("[WiFi] SSID not found");
#endif
            break;
        case WL_CONNECT_FAILED:
#ifdef PRINT
            Serial.print("[WiFi] Failed - WiFi not connected! Reason: ");
#endif
            return false;
            break;
        case WL_CONNECTION_LOST:
#ifdef PRINT
            Serial.println("[WiFi] Connection was lost");
#endif
            break;
        case WL_SCAN_COMPLETED:
#ifdef PRINT
            Serial.println("[WiFi] Scan is completed");
#endif
            break;
        case WL_DISCONNECTED:
#ifdef PRINT
            Serial.println("[WiFi] WiFi is disconnected");
#endif
            break;
        case WL_CONNECTED:
#ifdef PRINT
            Serial.println("[WiFi] WiFi is connected!");
            Serial.print("[WiFi] IP address: ");
            Serial.println(WiFi.localIP());
#endif
            connected = true;
            break;
        default:
#ifdef PRINT
            Serial.print("[WiFi] WiFi Status: ");
            Serial.println(WiFi.status());
#endif
            break;
        }
        delay(tryDelay);

        if (numberOfTries <= 0)
        {
#ifdef PRINT
            Serial.print("[WiFi] Failed to connect to WiFi!");
#endif
            // Use disconnect function to force stop trying to connect
            WiFi.disconnect();
            return false;
        }
        else
        {
            numberOfTries--;
        }
    }

#ifdef PRINT
    Serial.println("You're connected to the network");
    Serial.println();
#endif

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

        while (1)
            ;
    }

    Serial.println("You're connected to the MQTT broker!");
    Serial.println();
    return true;
}