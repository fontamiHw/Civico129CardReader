/*
  ArduinoMqttClient - WiFi Simple Sender

  This example connects to a MQTT broker and publishes a message to
  a topic once a second.

  The circuit:
  - Arduino MKR 1000, MKR 1010 or Uno WiFi Rev2 board

  This example code is in the public domain.
*/
#include "arduino_secrets.h"
#include "src/cardReader/mqtt/CardReaderMqtt.h"
#include "src/cardReader/nfc/CardReaderNfc.h"
#include "src/global.h"
#include <MFRC522.h>

const long interval = 3000;
unsigned long previousMillis = 0;
int count = 0;
bool connected = false;

#define SS_PIN 7
#define RST_PIN 9
#define MISO_PIN 10
#define MOSI_PIN 6
#define SCK_PIN 4

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
CardReaderMqtt *cardReaderMqtt;
CardReaderNfc *cardReaderNfc;
const char broker[] = BROKER_ADDRESS;
int port = BROKER_PORT;
///////end arduino_secrets.h
char msg[12];

void setup()
{
#ifdef PRINT
  Serial.begin(115200);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
#endif

  cardReaderMqtt = new CardReaderMqtt(SECRET_SSID, SECRET_PASS);
  cardReaderMqtt->addMqttParam(broker, port, MQTT_USER, MQTT_PASS);
  cardReaderNfc = new CardReaderNfc(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN, RST_PIN);
  cardReaderNfc->begin();

  // mqqt connected when Not blue
  pinMode(LED_MQTT, OUTPUT);
  digitalWrite(LED_MQTT, LOW);

  connected = cardReaderMqtt->connect();
  cardReaderMqtt->subscribe(MQTT_TOPIC_REGISTER);

  digitalWrite(LED_MQTT, HIGH);
  previousMillis = 0;
}

void sendMsg(const byte *buffer, byte bufferSize)
{
  int j = 0;
  for (byte i = 0; i < bufferSize; i++)
  {
    if (i == 0)
    {
      sprintf(&msg[j], "%02X", buffer[i]);
      j += 2;
    }
    else
    {
      sprintf(&msg[j], "-%02X", buffer[i]);
      j += 3;
    }
  }
  msg[j] = '\0';

  // send message
  cardReaderMqtt->sendMessage(MQTT_TOPIC_READ, msg);
  previousMillis = millis();
  digitalWrite(LED_MQTT, LOW);
}

void loop()
{

  if (connected)
  {
    // call poll() regularly to allow the library to send MQTT keep alives which
    // avoids being disconnected by the broker
    cardReaderMqtt->poll();

    const CardUid cardUid = cardReaderNfc->readCard();
    if (cardUid.error.code == ERROR_NONE)
    {
      Serial.print(F("In hex: "));
      sendMsg(cardUid.uid.uidByte, cardUid.uid.size);
    }

    if (previousMillis > 0)
    {
      unsigned long currentMillis = millis();

      if (currentMillis - previousMillis >= interval)
      {
        // save the last time a message was sent
        previousMillis = currentMillis;

        digitalWrite(LED_MQTT, HIGH);
        previousMillis = 0;
      }
    }

    cardReaderMqtt->handleSubscription();
  }
}