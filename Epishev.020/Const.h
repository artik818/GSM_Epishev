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

#define MAX_NUMBER_LENGTH 35 // ������������ ����� ����������� ������

//#define MaxNumbers 3 // ���������� ��������� ��� �������, ��� ������ ������ - ������ � �����������
#define StartCell 20 // ������ � �����������, � ������� ����� �������� ������ ������ ���������

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
	
	cswStartWork,		// ���������� � ������ ������
	cswWork,			// ����� � ������
	cswStartWarta,		// ���������� � ������
	cswWarta,			// ������
	cswStartAlarm,		// ���������� � ������������
	cswAlarm,			// ������������
	cswStartKontur,		// ���������� � �������� ������� ����� ��������� �� ������
	cswKontur,			// �������� ������� ����� ��������� �� ������

	cswStartBilling,	// ����� �������� �����
	cswBilling,			// �������� �����
	cswCantConnectBill,	// ��� ����������� ��������� � �������� ��� ��������� �������
	cswLittleMoney,		// ������� �� ����� ������ ����������� �������

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


	cswOK,  // ������� (�������) ��������� �������
	cswNOK, // ������� (�������) �� ���������
	cswEnd,
};

enum {
	cSettings = 20,
	cNumSetts,
	cZonesSetts,
	cProverkaScheta,
	cTerm01, 			// ������� ����������� � ������������ ���������� ��� ������� 1
	cTerm02, 			// ������� ����������� � ������������ ���������� ��� ������� 2
	cNumber01,			// ������ ������ � ���������� �������
	cNumber02,			// 
	cNumber03,			// 
	cNumber04,			// 
	cNumber05,			// 
	cNumber06,			// 
	cLast,				// ���������� ����� ����� ����� ����� ��������� ��� = cLast-cSettings
};

enum { cAllNumbersCount = cLast-cSettings }; // ���������� ����� ����� = cLast-cSettings

enum { cPrivilegeNone = 0};
enum { cPrivilegeFull = 1};
enum { cPrivilegeAllSMS = 2};
enum { cPrivilegeAlarmSMS = 4};


enum { cFirstSettsCount = 7};


#define MinBill 4
#define MaxBill 5

//-------------------------------------------

#endif
