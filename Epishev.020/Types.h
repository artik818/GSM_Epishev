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
	// �������� ������: 1-� +1; 2-� +2; 3-� +4; 4-� +8. (�� ��������� - 0)
	// ��� ������������ ���� ����� �� �����. ����� ��������� xor, �������� ������� 1 ���� ���� � �������� ��������� � 0 ���� � ����������
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
	char SMSPriv; // ���� ���������� = 0, �� ��� ���������, �� �������� ������ ��� ��������
	char SMSZones;
};

#endif
