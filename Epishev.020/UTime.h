#ifndef UTIME__H
#define UTIME__H

#include "UGlobals.h"

typedef unsigned int TTime;

TTime GetTime(void);
unsigned int Ticks(unsigned long int Time_ms);
void initTimer(void);

extern char WasBadVect; // хоть раз был переход по неправильному вектору. в принципе нужна только на стадии разраьотки для проверки баговитости системы
extern volatile unsigned int TimerVar5ms;

#endif
