
#include "UGlobals.h"

unsigned char DIns;
unsigned char DOuts = 0b11110000;

unsigned char IndSate;

//-------------------------------------------
char flIsReaded; 
char tmp_flIsReaded;
int tmpFromModemBufLen;
char tmpFromModemBuf[MAX_BUF];
int FromModemBufLen;
char FromModemBuf[MAX_BUF];
char PromFromModemBuf[MAX_BUF];

//-------------------------------------------
char ResCharM;
char ResCharP;

struct SPhoneNumber PhoneNumber[cAllNumbersCount];

// номера телефонов и абонентов !обязательно! должны быть обрамлены кавычками!
/*
struct SDefaults Defaults[cAllNumbersCount] = {
	{"\"Settings\"",  "\"00018001\""},
	{"\"NumSetts\"",  "\"00333321\""},
	{"\"UserSetts\"", "\"0000121203031515\""},
	{"\"ProverkaScheta\"", "\"*100#\""},
	{"\"Operator1\"", "\"+380972116686\""},
	{"\"Operator2\"", "\"+380504841620\""},
	{"\"Operator3\"", "\"+380504514232\""},
	{"\"Operator4\"", "\"000\""},
	{"\"Operator5\"", "\"+380973395213\""},
	{"\"Operator6\"", "\"000\""},
};
*/
struct SDefaults Defaults[cAllNumbersCount] = {
	{"\"Settings\"",  "\"18000000\""}, //!!!!!
	{"\"NumSetts\"",  "\"13333300\""},
	{"\"UserSetts\"", "\"1515030312120000\""},
	{"\"Reserv\"", "\"000\""},
	{"\"Datch1\"", "\"00050035\""}, //!!!!!
	{"\"Datch2\"", "\"10010045\""},
	//{"\"Datch1\"", "\"0000000\""},
	//{"\"Datch2\"", "\"0000000\""},
	//{"\"Operator1\"", "\"000\""}, //!!!!!
	//{"\"Operator1\"", "\"+380972116686\""}, //!!!!!
	{"\"Operator1\"", "\"+380913185246\""}, // Епишев Прогресс
	{"\"Operator2\"", "\"000\""},
	{"\"Operator3\"", "\"000\""},
	{"\"Operator4\"", "\"000\""},
	{"\"Operator5\"", "\"000\""},
	{"\"Operator6\"", "\"000\""},
};

struct SSettings Settings;

int CellNumber;

/*
char SMSText[MAX_BUF];
char SMSPriv;
char SMSZones;
*/
struct SSMSToSend SMSToSend;

char flMoreThenMax = 1; // денег было много - требуется проверка остатка
int Ostatok = 0; // остаток на счету после последней проверки

struct SFirstSetts FirstSetts[cFirstSettsCount] = {
	{"at+ipr=9600\r\n"},
	{"at+creg=1\r\n"},
	{"at+colp=1\r\n"},
	{"at+clip=1\r\n"},
	{"at+cmgf=1\r\n"},
	{"ate0\r\n"},
	{"at\r\n"},
};
int CurrentSet; // номер текущей команды из массива

long int baudRates[] = {
	9600,
	18200,
	38400,
	57600,
	115200
};
const int sizeOfBaudRates = sizeof(baudRates) / sizeof(baudRates[0]);
