#include "CardReaderNfc.h"

#include <SPI.h>
#include <stdio.h>
#include <MFRC522.h>

CardReaderNfc::CardReaderNfc()
{
}

CardReaderNfc::~CardReaderNfc()
{
}

CardReaderNfc::CardReaderNfc(int sckPin, int misoPin, int mosiPin, int ssPin, int rstPin)
{
    nfcPin.sckPin = sckPin;
    nfcPin.misoPin = misoPin;
    nfcPin.mosiPin = mosiPin;
    nfcPin.ssPin = ssPin;
    nfcPin.rstPin = rstPin;

    Serial.println("Initializing NFC reader...");
    rfid = new MFRC522(nfcPin.ssPin, nfcPin.rstPin); // Init MFRC522
}

void CardReaderNfc::begin()
{
    SPI.begin(nfcPin.sckPin, nfcPin.misoPin, nfcPin.mosiPin, nfcPin.ssPin); // Init SPI bus
    rfid->PCD_Init();
}

const CardUid CardReaderNfc::readCard()
{
    cardUid.error.code = ERROR_NONE;

    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if (!rfid->PICC_IsNewCardPresent())
    {
        cardUid.error.code = NO_CARD_PRESENT;
        return cardUid;
    }

    // Verify if the NUID has been readed
    if (!rfid->PICC_ReadCardSerial())
    {
        cardUid.error.code = ERROR_READ_FAILED;
        return cardUid;
    }

    MFRC522::PICC_Type piccType = rfid->PICC_GetType(rfid->uid.sak);

    int len = sizeof(MFRC522::Uid);
    char *src = (char *)&rfid->uid;
    char *dst = (char *)&cardUid.uid;
    memcpy(dst, src, len);
        
    // Halt PICC
    rfid->PICC_HaltA();

    // Stop encryption on PCD
    rfid->PCD_StopCrypto1();
    return cardUid;
}