#ifndef CONST__H
#define CONST__H

#include <avr\io.h>
#include <avr\iom128.h>
#include <string.h>
#include <setjmp.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#ifndef F_CPU
  #error !!!   UNDEFINED F_CPU   (Project->Options->General->Frequency) !!!!!
#else
  #define BAUD(BaudRate) (F_CPU/16/BaudRate)-1
#endif


#define MAX_BUF 150

#define MAX_NUMBER_LENGTH 35 // максимальна€ длина телефонного номера

//#define MaxNumbers 3 // количество телефонов дл€ дозвона, где перва€ €чейка - €чейка с настройками
#define StartCell 20 // €чейка с настройками, с которой нужно начинать читать номера телефонов

//-------------------------------------------
enum {
	cswIdle,
	cswStart,
	
	cswStartModemConnectionSpeed,
	cswModemConnectionSpeed01,
	cswModemConnectionSpeed02,
	cswModemConnectionSpeed03,
	
	cswStartInitModem,
	cswInitModem,
	
	cswWaitReady,
	cswERRORWaitReady,
	cswOKWaitReady,

	cswReadSettings,
	cswWriteSettings,
	cswERRORWriteSettings,
	cswENDWriteSettings,
	
	cswStartWork,		// подготовка к началу работы
	cswWork,			// сн€та с охраны
	cswStartWarta,		// подготовка к охрана
	cswWarta,			// охрана
	cswStartAlarm,		// подготовка к сигнализаци€
	cswAlarm,			// сигнализаци€
	cswStartKontur,		// подготовка к проверке контура перед переходом на охрану
	cswKontur,			// проверка контура перед переходом на охрану

	cswStartBilling,	// стврт проверка счета
	cswBilling,			// проверка счета
	cswCantConnectBill,	// нет возможности св€затьс€ с сервером дл€ получени€ остатка
	cswLittleMoney,		// остаток на счете меньше дозволенной границы

	csw005, // 
	csw010, // 
	csw020,
	csw025,
	csw030,
	csw040,
	csw050,
	csw060,
	csw070,
	csw080,
	csw081,
	csw082,
	csw083,
	csw090,
	csw100,


	cswOK,  // команда (функци€) выполнена успешно
	cswNOK, // команда (функци€) не выполнена
	cswEnd,
};

enum {
	cSettings = 20,
	cNumSetts,
	cZonesSetts,
	cProverkaScheta,
	cTerm01, 			// ”ставки минимальной и максимальной температур дл€ датчика 1
	cTerm02, 			// ”ставки минимальной и максимальной температур дл€ датчика 2
	cNumber01,			// перва€ €чейка с телефонным номером
	cNumber02,			// 
	cNumber03,			// 
	cNumber04,			// 
	cNumber05,			// 
	cNumber06,			// 
	cLast,				// количество €чеек всего можно будет посчитать как = cLast-cSettings
};

enum { cAllNumbersCount = cLast-cSettings }; // количество €чеек всего = cLast-cSettings

enum { cPrivilegeNone = 0};
enum { cPrivilegeFull = 1};
enum { cPrivilegeAllSMS = 2};
enum { cPrivilegeAlarmSMS = 4};


enum { cFirstSettsCount = 7};


#define MinBill 4
#define MaxBill 5

//-------------------------------------------

#endif
