#include <stdio.h>

#include "UGlobals.h"
#include "UFuncs.h"
#include "UAlgs.h"
#include "UTerm.h"



//------------------------------------------------------------------------------------


int main (void) {

LStart:

	cli();
	initTimer();
	//disable_interrupt();
	//SREG = SREG & 0b01111111;


flIsReaded=0; 
tmpFromModemBufLen = 0;
FromModemBufLen = 0;


	// инициализация входов/выходов
	InitInsOuts();
	MDelay(1000);
	
	SetIndBits(0b00000010);OutInd(); //!!!

	// инициализация СОМ портов
	USART1_Init(BAUD(57600));	// внешний порт
	USART0_Int_Init(BAUD(9600));	// модем
	MDelay(3000);
	SetIndBits(0b00000001);OutInd(); //!!!

	// включение модема
	ModemOnOff();
	MDelay(1000);
	
	sei();
	
	SetIndBits(0b00000010);OutInd(); //!!!
	
	if (IsDebug()) { //!#ifdef M_DEBUG
		PutPC("\r\nSTART\r\n");
	} //!#endif
	
	PutModem("AT\r\n");

	//MDelay(1000);

	//sei();

	TermInit();
	swMainAlg = cswStart;
	
	while (1) {

		DIn();

		if (IsDebug()) { //!#ifdef M_DEBUG
			if (USART1_Receive(&ResCharP)) {
				char Ret = ProcessKey(ResCharP);
				//if (Ret == 0x27) break; // выход
				if (Ret != 1) USART0_Transmit(ResCharP);
			}
		} //!#endif
		
		//ReadStringModem();
		//
		if (tmp_flIsReaded) {
			cli();
			strncpy(FromModemBuf, PromFromModemBuf, MAX_BUF-1);
			tmp_flIsReaded = 0;
			sei();
			flIsReaded = 1;
			
			if (IsDebug()) { //!#ifdef M_DEBUG
				PutPC(FromModemBuf);
				PutPC("\r\n");
			} //!#endif
		}
		
//		PutPC("PINF = ");
//		char tmpS[20];
//		THERM_INPUT_MODE6();
//		therm_delay(us(14));
//		tmpS[0] = THERM_PIN;// & (1<<PF0);
		/*
		tmpS[0] = PINF & (1<<PF0);
		tmpS[1] = PINF & (1<<PF1);
		tmpS[2] = PINF & (1<<PF2);
		tmpS[3] = PINF & (1<<PF3);
		tmpS[4] = PINF & (1<<PF4);
		tmpS[5] = PINF & (1<<PF5);
		tmpS[6] = PINF & (1<<PF6);
		tmpS[7] = PINF & (1<<PF7);
		*/
//		PutPCCharsN(tmpS, 1);
//		PutPC("<\r\n");
		
		// --------- отработка алгоритмов
		char retMain = MainAlg();
		if (retMain == 100) goto LStart;

		if (flIsReaded != 0) {
			flIsReaded = 0;
		}
		// --------- END отработка алгоритмов

		DOut();
		OutInd();

		//TermAlg(0);
		//TermAlg(1);
		/*
		if (Terms[0].swTermAlg == cswIdle) {
			PutPC(StrTherm);
			memset(Terms[0].StrTherm, 0, STR_TERM_LEN);
			Terms[0].swTermAlg = cswStart;
		}
		*/
		/*
		if (swTermAlg == cswIdle) {
			//Temper.InByte[1]= 0xf0; // 
			//Temper.InByte[0]= 0x0f; // 
			//Temper.Value = Temper.Value >> 4;
			//sprintf(StrTherm, "%d C\r", Temper.Value);

			PutPC(StrTherm);
			memset(StrTherm, 0, STR_TERM_LEN);
			swTermAlg = cswStart;
		}
		*/
	}

	ModemOnOff();
	MDelay(4000);
	return 0;
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------



		/*
		if (USART0_Receive(&ResCharM)) {
			USART1_Transmit(ResCharM);
		}
		*/
/*
		switch (tmpSW) {
			case 0:
				tmpTime = GetTime();
				tmpSW = 1;
			break;
			case 1:
				tmpTime2 = GetTime();
				if ((tmpTime2 - tmpTime) >= Ticks(1000)) {
					PutPC("W");
					tmpSW = 0;
				}
			break;
		}
*/
		/*
		if (flIsReaded != 0) {
			PutPC(">");
			if (strstr(FromModemBuf, "OK") != NULL) {
				PutPC("O");
			}
			else
			if (strstr(FromModemBuf, "RING") != NULL) {
				PutPC("R");
			}
			else
			if (strstr(FromModemBuf, "ERROR") != NULL) {
				PutPC("E");
			}
			else
			if (strstr(FromModemBuf, "+CPIN: READY") != NULL) {
				PutPC("C");
			}
			
			PutPC("\r\n");
			flIsReaded = 0;
		}
		*/

		// --------- отработка алгоритмов
/*
		Work();

		IsModemOn();
		
		char r = ReadSettings();
		if ((r!=0) && (swReadSettings==cswNOK)) { // ошибка сработки
			char tmpCh[10];
			tmpCh[0] = r+0x30;
			tmpCh[1] = 0;
			PutPC("er ");
			PutPC(tmpCh);
			PutPC("\r\n");
			swReadSettings=cswIdle;
		}
		if ((r==0) && (swReadSettings==cswOK)) { // отработала нормально
			PutPC("ok\r\n");
			PutPC(PhoneNumber00);
			PutPC("\r\n");
			PutPC(PhoneNumber01);
			PutPC("\r\n");
			PutPC(PhoneNumber02);
			PutPC("\r\n");
			swReadSettings=cswIdle;
		}

*/
		/*
		char tmpCh22[10];
		tmpCh22[0] = DIns;
		tmpCh22[1] = 0;
		tmpCh22[2] = 0;
		PutPC(tmpCh22);
		*/
