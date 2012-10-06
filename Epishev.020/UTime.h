#ifndef UTIME__H
#define UTIME__H

#include "UGlobals.h"

typedef unsigned int TTime;

TTime GetTime(void);
unsigned int Ticks(unsigned long int Time_ms);
void initTimer(void);

extern char WasBadVect; // ���� ��� ��� ������� �� ������������� �������. � �������� ����� ������ �� ������ ���������� ��� �������� ����������� �������
extern volatile unsigned int TimerVar5ms;

#endif
