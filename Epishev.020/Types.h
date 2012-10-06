#ifndef TYPES__H
#define TYPES__H

#include "Const.h"

typedef unsigned char u08;

struct SPhoneNumber {
	char Number[MAX_NUMBER_LENGTH];
	char IsActive;
	char Privilege;
	char Zones;
};

struct SSettings {
	// инверсия входов: 1-й +1; 2-й +2; 3-й +4; 4-й +8. (по умолчанию - 0)
	// при накладывании этой маски на дискр. входы операцией xor, получаем всегада 1 если вход в активном состоянии и 0 если в неактивном
	unsigned char DInInversion;
	unsigned char AlarmTime_s;
};

struct SDefaults {
	char Abonent[MAX_NUMBER_LENGTH];
	char Number[MAX_NUMBER_LENGTH];
};

struct SFirstSetts {
	char CMDName[20];
};

struct SSMSToSend {
	char SMSText[MAX_BUF];
	char SMSPriv; // если привелегии = 0, то СМС неактивна, не содержит данных для отправки
	char SMSZones;
};

#endif
