#ifndef CARDREADERNFC_H
#define CARDREADERNFC_H
#include <MFRC522.h>

#pragma once


typedef struct NfcPin_t
{
    int sckPin;
    int misoPin;
    int mosiPin;
    int ssPin;
    int rstPin;
} NfcPin;

typedef enum Error_e
{
    ERROR_NONE = 0,
    NO_CARD_PRESENT = 1,
    ERROR_CONNECTION_FAILED = 2,
    ERROR_READ_FAILED = 3,
    ERROR_UNKNOWN = 99
} Error_e;

typedef struct Error_t
{
    Error_e code;
    String message;
} Error;


typedef struct CardUid_t
{
    MFRC522::Uid uid;
    Error_t error;
} CardUid;

class CardReaderNfc {
public:
    CardReaderNfc(int sckPin, int misoPin, int mosiPin, int ssPin, int rstPin);
    ~CardReaderNfc();
    const CardUid readCard();
    void begin();

private:
    CardUid cardUid;
    MFRC522 *rfid; // Instance of the class
    NfcPin nfcPin;        
    CardReaderNfc();// not allowed empty constructor

};

#endif