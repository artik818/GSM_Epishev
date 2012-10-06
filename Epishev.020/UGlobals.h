#ifndef UGLOBALS__H
#define UGLOBALS__H

#include "const.h"
#include "types.h"

extern unsigned char DIns;
extern unsigned char DOuts;

extern unsigned char IndSate;

//-------------------------------------------
extern char flIsReaded; 
extern char tmp_flIsReaded;
extern int tmpFromModemBufLen;
extern char tmpFromModemBuf[MAX_BUF];
extern int FromModemBufLen;
extern char FromModemBuf[MAX_BUF];
extern char PromFromModemBuf[MAX_BUF];

//-------------------------------------------
extern char ResCharM;
extern char ResCharP;

extern struct SPhoneNumber PhoneNumber[cAllNumbersCount];
extern struct SDefaults Defaults[cAllNumbersCount];

extern struct SSettings Settings;

/*
extern char SMSText[MAX_BUF];
extern char SMSPriv;
extern char SMSZones;
*/
extern struct SSMSToSend SMSToSend;

extern int CellNumber;

extern char flMoreThenMax;
extern int Ostatok;

extern struct SFirstSetts FirstSetts[cFirstSettsCount];
extern int CurrentSet;

extern long int baudRates[];
extern const int sizeOfBaudRates;

#endif
