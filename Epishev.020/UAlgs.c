
#include <stdlib.h>
#include <stdio.h>

#include "UAlgs.h"


//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// ��� ������� ������� �����: swIsModemOn = cswStart;
// ���� �� 1000 �� ��� ������� ������ �� ������, �� ���������� ������ 1
// ��� ������ (������������) ��������� swIsModemOn = cswIdle;
TTime TPrevIsModemOn;
int swIsModemOn = cswIdle;
char IsModemOn(void) {
	char RetVal = 0;
	switch(swIsModemOn) {
		case cswIdle:
		break;
		
		case cswStart:
			DoCommand(&TPrevIsModemOn, "at+cpin?", &swIsModemOn, csw010);
		break;
		
		case csw010:
			WaitAnswer(TPrevIsModemOn, "+CPIN: READY", 2000, &swIsModemOn, cswOK, cswIdle);
		break;
		
		case cswOK:
			RetVal = 1;
		break;
		
		case cswEnd:
			swIsModemOn = cswIdle;
		break;
	}
	return RetVal;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

void UnPackSettings(char *StrSettings) {
	char tmpStr[10];
	unsigned char tmpVal = 0;
	int NumLen = strlen(StrSettings);
	if (NumLen < 5) {
		Settings.DInInversion = 0;
		Settings.AlarmTime_s = 60;
		return;
	}
	
	// ����� ����� ������ � �������� (3 �������)
	strncpy(tmpStr, &(StrSettings[0]), 3);
	tmpStr[3] = 0;
	tmpVal = atoi(tmpStr);
	Settings.AlarmTime_s = tmpVal;
	if (Settings.AlarmTime_s < 30) Settings.AlarmTime_s = 30;

	// �������� (2 �������)
	strncpy(tmpStr, &(StrSettings[3]), 2);
	tmpStr[2] = 0;
	tmpVal = atoi(tmpStr);
	Settings.DInInversion = (tmpVal<<4);
}
//------------------------------------------------------------------------------------

void UnPackNumSetts(char *StrSettings) {
	char tmpStr[10];
	unsigned char tmpVal = 0;
	int NumLen = strlen(StrSettings);
	if (NumLen < 6) {
		return;
	}
	
	for (int CellNumber=cNumber01; CellNumber<cLast; ++CellNumber) {
		//strncpy(tmpStr, &(StrSettings[NumLen-(CellNumber-cNumber01)-1]), 1);
		strncpy(tmpStr, &(StrSettings[CellNumber-cNumber01]), 1);
		tmpStr[1] = 0;
		tmpVal = atoi(tmpStr);
		char Priv = cPrivilegeNone;
		if (tmpVal == 1) Priv = cPrivilegeFull;
		else if (tmpVal == 2) Priv = cPrivilegeAllSMS;
		else if (tmpVal == 3) Priv = cPrivilegeAlarmSMS;
		PhoneNumber[CellNumber-cSettings].Privilege = Priv;
	}
}
//------------------------------------------------------------------------------------

void UnPackZonesSetts(char *StrSettings) {
	char tmpStr[10];
	unsigned char tmpVal = 0;
	int NumLen = strlen(StrSettings);
	if (NumLen < 12) {
		return;
	}
	
	for (int CellNumber=cNumber01; CellNumber<cLast; ++CellNumber) {
		strncpy(tmpStr, &(StrSettings[(CellNumber-cNumber01)*2]), 2);
		tmpStr[2] = 0;
		tmpVal = atoi(tmpStr);
		PhoneNumber[CellNumber-cSettings].Zones = tmpVal;
	}
}
//------------------------------------------------------------------------------------

void UnPackTermSetts(char *StrSettings, int NumTerm) {
	char tmpStr[10];
	int tmpVal = 0;
	int NumLen = strlen(StrSettings);
	if (NumLen < 8) {
		Terms[NumTerm].MINLim = -55;
		Terms[NumTerm].MAXLim = 200;
		Terms[NumTerm].flEnable = 0;
		return;
	}
	
	Terms[NumTerm].flEnable = 1;
	// Tmin - 0 �������
	strncpy(tmpStr, &(StrSettings[1]), 3);
	tmpStr[3] = 0;
	tmpVal = atoi(tmpStr);
	if (StrSettings[0] == '1') tmpVal = 0 - tmpVal;
	Terms[NumTerm].MINLim = tmpVal;

	// Tmax - 4 �������
	strncpy(tmpStr, &(StrSettings[5]), 3);
	tmpStr[3] = 0;
	tmpVal = atoi(tmpStr);
	if (StrSettings[4] == '1') tmpVal = 0 - tmpVal;
	Terms[NumTerm].MAXLim = tmpVal;
}
//------------------------------------------------------------------------------------

TTime TPrevWriteSettings;
int swWriteSettings = cswIdle;
char WriteSettings(void) {
	char CellNumberStr[MAX_NUMBER_LENGTH];
	char tmpCh[MAX_BUF];
	char RetWaitAnswer;
	char RetVal = 1; // ��� �������� - ��� ������������� ����������
	switch(swWriteSettings) {
		case cswIdle:
		break;

		case cswStart: // ����� ���������� ����� �� �����
			DoCommand(&TPrevWriteSettings, "AT+CPBS=\"SM\"\r\n", &swWriteSettings, csw010);
			CellNumber = cSettings;
		break;
		case csw010: // ���� ������ �� ������
			if (WaitAnswer(TPrevWriteSettings, "OK", 2000, &swWriteSettings, csw020, cswNOK) == 4) RetVal = 4;
		break;
		
		case csw020: // ����� ������
			itoa(CellNumber, CellNumberStr, 10);
			strncpy(tmpCh, "AT+CPBW=", MAX_BUF-1);
			strncat(tmpCh, CellNumberStr, MAX_BUF-1);
			strncat(tmpCh, ",", MAX_BUF-1);
			strncat(tmpCh, Defaults[CellNumber-cSettings].Number, MAX_BUF-1);
			strncat(tmpCh, ",,", MAX_BUF-1);
			strncat(tmpCh, Defaults[CellNumber-cSettings].Abonent, MAX_BUF-1);
			strncat(tmpCh, "\r\n", MAX_BUF-1);
			if (IsDebug()) { //!#ifdef M_DEBUG
				PutPC(tmpCh);
			} //!#endif
			DoCommand(&TPrevWriteSettings, tmpCh, &swWriteSettings, csw030);
		break;
		case csw030: // ���� ������ �� ������
			RetWaitAnswer = WaitAnswer(TPrevWriteSettings, "OK", 3000, &swWriteSettings, csw020, cswNOK);
			if (RetWaitAnswer == 4) {
				RetVal = 4; 
				break;
			}
			else
			if (RetWaitAnswer == 1) {
				CellNumber++;
				if (CellNumber >= cLast) { // �������� ������ �� �����
					swWriteSettings = cswOK;
				}
			}
		break;
		
		case cswOK:
			RetVal = 0; // ��������� - ��� � �������
		break;
		case cswNOK:
			RetVal = 4; // ���-�� �� ���, ��� ������ � ...
		break;
		case cswEnd:
			swWriteSettings = cswIdle;
		break;
	}
	return RetVal;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// ������� ������ �������� � ���������� ��������������� ����������.
// ���������� 0 - ���� ���������� ���������. ����� ��������� ��������, ������� - swReadSettings = cswIdle 
// ���� ���������� 1 � ��������� �� � cswNOK, �� ��� ��������
// ���� ���������� 1 � ��������� � cswNOK, �� - ��� ������ ������� � ErrorReadSettings
// ErrorReadSettings = 0; - ��� ������
// ErrorReadSettings = 3; // ������� ������ ���������� ����� ��� �����
// ErrorReadSettings = 4; // ������� ������ ������ � ����������� 
// ErrorReadSettings = 5; // ��� 20-� ������ � ����� => ���� ���������� ������ ��������� ���������
// ErrorReadSettings = 6; // ��� 21-� ������ � ����� => ���� ���������� ������ ��������� ���������
// ErrorReadSettings = 7; // ��� 22-� ������ � ����� => ���� ���������� ������ ��������� ���������
TTime TPrevReadSettings;
int swReadSettings = cswIdle;
char ErrorReadSettings = 0; 
char ReadSettings(void) {
	char CellNumberStr[MAX_NUMBER_LENGTH];
	char tmpCh[MAX_NUMBER_LENGTH];
	char RetWaitAnswer;
	char RetVal = 1; // ��� �������� - ��� ������������� ����������
	switch(swReadSettings) {
		case cswIdle:
			ErrorReadSettings = 0;
		break;
	
		case cswStart: // ����� ���������� ����� �� �����
			DoCommand(&TPrevReadSettings, "AT+CPBS=\"SM\"\r\n", &swReadSettings, csw010);
			CellNumber = cSettings;
		break;
		case csw010: // ���� ������ �� ������
			if (WaitAnswer(TPrevReadSettings, "OK", 2000, &swReadSettings, cswMarkDelay01, cswNOK) == 4) {
				ErrorReadSettings = 3; // ������� ������ ���������� ����� ��� �����
				RetVal = ErrorReadSettings;
			}
		break;
		
		// ���������� ����� ��������� ���������� ����� �������
		case cswMarkDelay01:
			MarkDelay(&TPrevReadSettings, &swReadSettings, cswWaitDelay01);
		break;
		case cswWaitDelay01:
			WaitDelayAndGoTo(TPrevReadSettings, 1000, &swReadSettings, csw020);
		break;
		
		//----------------- ������ ���������� ������ �� �������� ������ --------------------------
		case csw020: 
			itoa(CellNumber, CellNumberStr, 10);
			strncpy(tmpCh, "AT+CPBR=", MAX_NUMBER_LENGTH-1);
			strncat(tmpCh, CellNumberStr, MAX_NUMBER_LENGTH-1);
			strncat(tmpCh, "\r\n", MAX_NUMBER_LENGTH-1);
			DoCommand(&TPrevReadSettings, tmpCh, &swReadSettings, csw030);
		break;
		case csw030: // ���� ������ �� ������
			RetWaitAnswer = WaitAnswer(TPrevReadSettings, "+CPBR: ", 2000, &swReadSettings, csw020, cswNOK);
			if (RetWaitAnswer == 4) {
				ErrorReadSettings = 4; // ������� ������ ���������� ����� ��� �����
				RetVal = ErrorReadSettings;
			}
			else
			if (RetWaitAnswer == 1) {
				char *tmpBegStr = &(FromModemBuf[11]);
				char *tmpEndStr = strstr(tmpBegStr, "\"");
				*tmpEndStr = 0;
				strncpy(PhoneNumber[CellNumber-cSettings].Number, tmpBegStr, MAX_NUMBER_LENGTH-1);
				// ----- �������� � ����������� �� ������
				if (CellNumber == cSettings) UnPackSettings(PhoneNumber[CellNumber-cSettings].Number);
				if (CellNumber == cNumSetts) UnPackNumSetts(PhoneNumber[CellNumber-cSettings].Number);
				if (CellNumber == cZonesSetts) UnPackZonesSetts(PhoneNumber[CellNumber-cSettings].Number);
				if (CellNumber == cProverkaScheta) {
					if (strncmp(PhoneNumber[CellNumber-cSettings].Number, "000", MAX_NUMBER_LENGTH-1) == 0) PhoneNumber[CellNumber-cSettings].IsActive = 0;
					else PhoneNumber[CellNumber-cSettings].IsActive = 1;
				}
				if (CellNumber == cTerm01) UnPackTermSetts(PhoneNumber[CellNumber-cSettings].Number, 0);
				if (CellNumber == cTerm02) UnPackTermSetts(PhoneNumber[CellNumber-cSettings].Number, 1);
				if (CellNumber >= cNumber01) {
					if (strncmp(PhoneNumber[CellNumber-cSettings].Number, "000", MAX_NUMBER_LENGTH-1) == 0) PhoneNumber[CellNumber-cSettings].IsActive = 0;
					else PhoneNumber[CellNumber-cSettings].IsActive = 1;
				}
				// ----- END �������� � ����������� �� ������
				
				CellNumber++;
				if (CellNumber >= cLast) { // �������� ������ �� �����
					swReadSettings = cswOK;
					break;
				}
			}
		break;
		//----------------- END ������ ���������� ������ �� �������� ������ ----------------------
		
		case cswOK:
			RetVal = 0; // ��������� - ��� � �������
		break;
		case cswNOK:
			RetVal = ErrorReadSettings; // ���-�� �� ���, ��� ������ � ...
		break;
		case cswEnd:
			ErrorReadSettings = 0;
			swReadSettings = cswIdle;
		break;
	}
	return RetVal;
}

//------------------------------------------------------------------------------------
// �������� ������
void SirenaOn(void) {
	DOuts = DOuts & 0b11101111; 
}
//------------------------------------------------------------------------------------
// ��������� ������
void SirenaOff(void) {
	DOuts = DOuts | 0b00010000; 
}
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// �������� �����, ���������� ������� ��� ���� ������
void SrabotkaOn(void) {
	DOuts = DOuts & 0b11011111; 
}
//------------------------------------------------------------------------------------
// ��������� �����, ���������� ������� ��� ���� ������
void SrabotkaOff(void) {
	DOuts = DOuts | 0b00100000; 
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int GetNumberFromBillString(char *BillStr, char *StrNumber) {
	char tmpCh[MAX_BUF];
	int CifraIndex = 0;
	char flWasCifra = 0;
	
	BillStr = strstr(BillStr, "\"");
	if (BillStr == NULL) return 0;
	
	for (int i=0; i<strlen(BillStr); ++i) {
		if (i >= MAX_BUF-5) {
			tmpCh[0] = '0';
			tmpCh[1] = 0;
			break;
		}
		
		if ((BillStr[i] >= '0') && (BillStr[i] <= '9')) { // ��� �����
			tmpCh[CifraIndex] = BillStr[i];
			++CifraIndex;
			flWasCifra = 1;
		}
		else { // �����-�� ����� ������ - ��� �� �����
			if (flWasCifra == 1) { // �������� ����� �� �����
				tmpCh[CifraIndex] = 0;
				break;
			}
		}
	}
	
	int RetVal = 0;
	if (flWasCifra) {
		RetVal = atoi(tmpCh);
	}
	return RetVal;
}
//------------------------------------------------------------------------------------

int swTryBilling = cswIdle;
TTime TTryBilling;
char TryBilling(void) {
	char RetWaitAnswer;
	char RetVal = 1; // ��� �������� - ��� ������������� ����������
	char cmdSendSMS[MAX_BUF];
	switch (swTryBilling) {
		case cswIdle:
		break;
		
		case cswStart:
			if (PhoneNumber[cProverkaScheta-cSettings].IsActive == 0) {
				swTryBilling = cswOK;
				break;
			}
			strncpy(cmdSendSMS, "at+cusd=1,\"", MAX_BUF-1);
			strncat(cmdSendSMS, PhoneNumber[cProverkaScheta-cSettings].Number, MAX_BUF-1);
			strncat(cmdSendSMS, "\",15\r\n", MAX_BUF-1);
			DoCommand(&TTryBilling, cmdSendSMS, &swTryBilling, csw010);
		break;
		
		case csw010:
			RetWaitAnswer = WaitAnswer(TTryBilling, "+CUSD: ", 10000, &swTryBilling, csw020, cswCantConnectBill);
		break;
		
		case csw020: // ������ �������
			//Ostatok = GetNumberFromBillString(FromModemBuf);
			if (Ostatok < MinBill) swTryBilling = cswLittleMoney;
			else swTryBilling = cswOK;
		break;
		
		case cswCantConnectBill: // ��� ����������� ��������� � �������� ��� ��������� �������
			RetVal = 10;
		break;
		
		case cswLittleMoney: // ������� �� ����� ������ ����������� �������
			RetVal = 11;
		break;
		
		case cswOK: // ���������� - ������� � �����
			RetVal = 0;
		break;
		
	}
	return RetVal;
}
//------------------------------------------------------------------------------------

//������� ��� ��������� �� ������������������ ������.
//Text � ����� ���������;
////����������: 0 � ��� ������, � ������ ������ � �� ���.
struct SSMSToSend SMSToSendAlg;
TTime TPrevSendSMS;
int swSMS = cswIdle;
char SendSMS() {
	char cmdSendSMS[MAX_BUF];
	switch (swSMS) {
		case cswIdle:
			if (SMSToSend.SMSPriv) {
				memcpy(&SMSToSendAlg, &SMSToSend, sizeof(SMSToSend));
				SMSToSend.SMSPriv = 0;
				swSMS = cswStart;
			}
		break;

		case cswStart:
			CellNumber = cNumber01;
			swSMS = cswMarkDelay00;
		break;
		
		case cswMarkDelay00:
			MarkDelay(&TPrevSendSMS, &swSMS, cswWaitDelay00);
		break;
		case cswWaitDelay00:
			WaitDelayAndGoTo(TPrevSendSMS, 3000, &swSMS, csw010);
		break;
		
		case csw010:
			if (CellNumber >= cLast) { // ��������� ��� ���� ���������
				swSMS = cswEnd;
				break;
			}
			if (
				(PhoneNumber[CellNumber-cSettings].IsActive==0) || // ���������
				((PhoneNumber[CellNumber-cSettings].Privilege & SMSToSendAlg.SMSPriv) == 0) || // �� �������� ������������ ��������� ������ ���
				!((SMSToSendAlg.SMSZones&(PhoneNumber[CellNumber-cSettings].Zones<<4)) & 0b11110000) // �� ��������� ���� ������ ����� ������
				) {
				CellNumber++;
			}
			else {
				strncpy(cmdSendSMS, "at+cmgs=\"", MAX_BUF-1);
				strncat(cmdSendSMS, PhoneNumber[CellNumber-cSettings].Number, MAX_BUF-1);
				strncat(cmdSendSMS, "\"\r\n", MAX_BUF-1);
				DoCommand(&TPrevSendSMS, cmdSendSMS, &swSMS, cswWaitDelay01);
			}
		break;
		/*case cswMarkDelay01:
			MarkDelay(&TPrevSendSMS, &swSMS, cswWaitDelay01);
		break;*/
		case cswWaitDelay01:
			//WaitDelayAndGoTo(TPrevSendSMS, 1000, &swSMS, cswDoneDelay01);
			WaitDelayAndGoTo(TPrevSendSMS, 1000, &swSMS, cswDoneDelay02);
		break;
		case cswDoneDelay01:
			strncpy(cmdSendSMS, "\r\n", MAX_BUF-1);
			DoCommand(&TPrevSendSMS, cmdSendSMS, &swSMS, cswWaitDelay02);
		break;
		case cswWaitDelay02:
			WaitDelayAndGoTo(TPrevSendSMS, 3000, &swSMS, cswDoneDelay02);
		break;
		case cswDoneDelay02:
			strncpy(cmdSendSMS, SMSToSendAlg.SMSText, MAX_BUF-1);
			strncat(cmdSendSMS, "\x1A", MAX_BUF-1); // 26 = ����� ����� = ^Z
			DoCommand(&TPrevSendSMS, cmdSendSMS, &swSMS, csw020);
		break;
		case csw020: // ������ ���� "+CMGS:"
			WaitAnswer(TPrevSendSMS, "+CMGS:", 15000, &swSMS, csw030, csw030);
		break;
		case csw030: // ���� "OK" � �� ����� �� ������ (����� 0) ��������� � ���������� ������
			if (WaitAnswer(TPrevSendSMS, "OK", 5000, &swSMS, csw010, csw010)) {
				CellNumber++;
			}
		break;
		case cswEnd:
			swSMS = cswIdle;
		break;
	}

	return 0;
}
//------------------------------------------------------------------------------------

int swIndik = cswIdle;
int old_swIndik = cswIdle;
int AzbukaIndex = 0;
int swAzbuka = cswStart;
TTime TIndik;
unsigned int AzbukaMorze[10];
char Indik(void) {
	if (swIndik != cswIdle) {
		if (old_swIndik != swIndik) { // ����� ���������
			AzbukaIndex = 0;
			switch (swIndik) {
				case cswIdle:
				break;
				
				case cswStart:
				break;
				
				case cswWaitReady:
					AzbukaMorze[0] = 500;
					AzbukaMorze[1] = 500;
					AzbukaMorze[2] = 1500;
					AzbukaMorze[3] = 0;
				break;
				
				case cswERRORWaitReady:
					AzbukaMorze[0] = 500;
					AzbukaMorze[1] = 500;
					AzbukaMorze[2] = 500;
					AzbukaMorze[3] = 500;
					AzbukaMorze[4] = 500;
					AzbukaMorze[5] = 0;
				break;
				
				case cswReadSettings:
					AzbukaMorze[0] = 1500;
					AzbukaMorze[1] = 1500;
					AzbukaMorze[2] = 500;
					AzbukaMorze[3] = 0;
				break;
				
				case cswStartWork: // ����� "������" - �������� ��� �����
					AzbukaMorze[0] = 0;
					SetIndBits(0);
					swIndik = cswIdle;
				break;
			}
		}
		else { // ������ ��������� - ���������� ������
			switch (swAzbuka) {
				case cswStart:
					OnIndBit(0b00000001);
					TIndik = GetTime();
					swAzbuka = csw010;
				break;
				case csw010:
					if ((GetTime() - TIndik) >= Ticks(AzbukaMorze[AzbukaIndex])) { 
						swAzbuka = cswOK;
					}
				break;
				case cswOK:
					OffIndBit(0b00000001);
					TIndik = GetTime();
					swAzbuka = csw020;
				break;
				case csw020:
					if ((GetTime() - TIndik) >= Ticks(500)) { 
						AzbukaIndex++;
						if (AzbukaMorze[AzbukaIndex] == 0) swAzbuka = cswEnd; // ��������� ����� ������� - ������ �����
						else swAzbuka = cswStart;
					}
				break;
		//------------------------
				case cswEnd: // ������
					TIndik = GetTime();
					swAzbuka = csw030;
				break;
				case csw030:
					if ((GetTime() - TIndik) >= Ticks(500)) { 
						AzbukaIndex = 0;
						swAzbuka = cswStart;
					}
				break;
			}
		}
	}
	old_swIndik = swIndik;
	return 0;
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

char GetNumberPrivileges(char *InPhoneNumber) {
	char RetVal = 0;
	for (int CellNumber=cNumber01; CellNumber<cLast; ++CellNumber) {
		if (strncmp(InPhoneNumber, PhoneNumber[CellNumber-cSettings].Number, MAX_NUMBER_LENGTH-1)==0) {
			RetVal = PhoneNumber[CellNumber-cSettings].Privilege;
			break;
		}
	}
	return RetVal;
}
//------------------------------------------------------------------------------------

char CountRings = 0;
char flWasPrivUserCall = 0;
char AlgCountRings(void) {
	if (flIsReaded != 0) {
		if (strstr(FromModemBuf, "+CLIP: \"") != NULL) {
			char *tmpBegStr = &(FromModemBuf[8]);
			char *tmpEndStr = strstr(tmpBegStr, "\"");
			*tmpEndStr = 0;
			if (GetNumberPrivileges(tmpBegStr) != cPrivilegeFull) {
				CountRings = 0;
				PutModem("ath\r\n");
			}
		} else
		if (strstr(FromModemBuf, "RING") != NULL) {
			if (CountRings >= 3) {
				PutModem("ath\r\n");
				flWasPrivUserCall = 1;
				CountRings = 0;
			}
			else {
				CountRings++;
			}
		}
		else
		if (strstr(FromModemBuf, "NO CARRIER") != NULL) {
			CountRings = 0;
		}
	}
	return 0;
}
//------------------------------------------------------------------------------------

char tmpStr2[100]; // ��� ���������� ����������� ����������
//------------------------------------------------------------------------------------
TTime TOutTermToPC;
int swOutTermToPC = cswStart;
void OutTermToPC(void) {
	switch (swOutTermToPC) {
		case cswIdle:
		break;
		
		case cswStart:
			TOutTermToPC = GetTime();
			swOutTermToPC = csw010;
		break;
			
		case csw010:
			if (GetTime() - TOutTermToPC > Ticks(5000)) {
				MakeTerm(tmpStr2);
				if (strlen(tmpStr2) > 0) {
					PutPC(tmpStr2);
					PutPC("\n\r");
				}
				swOutTermToPC = cswStart;
			}
		break;
	}
}

//------------------------------------------------------------------------------------
int swWork = cswIdle;
int swAlarm = cswIdle;
int swCall = cswIdle;
TTime TPrevWork, TPrevWorkAlarm, TPrevWorkCall;
TTime TPrevKontur;
char cmdCallNumber[MAX_NUMBER_LENGTH];
unsigned long int AlarmTime_ms;

//------------------------------------------------------------------------------------

char flSirenaOtkaz = 0;
TTime TOtpalDatchik;
void AlgMakeTermSirena(void) {
	for (int i=0; i<THERM_COUNT; ++i) { // �������� ������ �������� ������
		TermAlg(i);
		if (Terms[i].swTermAlg == cswIdle) Terms[i].swTermAlg = cswStart;
	}
	// ���� ������ �� ���� ������� ����� ������� �� ������ ��������(�� ���� �������), �� ����������� ������ �� ��������
	// ���� ����� ��������, �� �������� �����
	if (!flSirenaOtkaz) {
		for (int i=0; i<THERM_COUNT; ++i) {
			if (!Terms[i].flEnable) continue; // ���� ������ �� � ������, �� �� ���� � �� ��������
			if ((Terms[i].trigOtkaz == 1) || (Terms[i].trigCRCOtkaz == 1) || (Terms[i].trigMINLim == 1) || (Terms[i].trigMAXLim == 1)) {
				flSirenaOtkaz = 1;
				TOtpalDatchik = GetTime();
			}
		}
	}
	// ���� ��������� �������, ������ ����� ����� ���������� ������������� ������ � ���
	// ���� ������� �� �����, ������ ��� ������ ��������� �������� �� ������ �� �����������-�� ��������
	if (flSirenaOtkaz) {
		if ((GetTime() - TOtpalDatchik) >= Ticks(2000)) {
			MakeTerm(tmpStr2);
			for (int i=0; i<THERM_COUNT; ++i) {
				Terms[i].trigOtkaz = 0;
				Terms[i].trigCRCOtkaz = 0;
				Terms[i].trigMINLim = 0;
				Terms[i].trigMAXLim = 0;
			}
			if (!IsDebug()) { //!#ifdef M_DEBUG
				//#ifndef M_DEBUG // ���� �������, �� ��� �� ��������
				SMSToSend.SMSPriv = cPrivilegeFull|cPrivilegeAllSMS|cPrivilegeAlarmSMS;
				SMSToSend.SMSZones = 0xFF; // ��� ����
				strcpy(SMSToSend.SMSText, tmpStr2);
			} //!#endif
			
			swAlgTermSirena = cswStart;
			
			if (IsDebug()) { //!#ifdef M_DEBUG
				PutPC(tmpStr2);
				PutPC("\n\r");
			} //!#endif

			flSirenaOtkaz = 0; // ����� ��������� �� ������ ������� �� ��������
		}
	}
}

char Work(void) {
	
	AlgMakeTermSirena();
	AlgTermSirena();
	
	OutTermToPC();
	
	AlgCountRings();
	
	switch (swWork) {
		case cswIdle:
		break;
		
		case cswStart:
		break;
		
// RABOTA
// � ���� ������:
// ���� �������� ������ - ������ ��� ����� 7 ������
// ���� ��������� ��� � ����� ������� �� ����������� ������� - ������� ����� �� "������"
		case cswStartWork:
			CountRings = 0;
			//! ��������� �������� "��������� ���" � ������� "����� � ������" �� ������ ������� ��������
			SMSToSend.SMSPriv = cPrivilegeFull|cPrivilegeAllSMS;
			SMSToSend.SMSZones = 0xFF; // ����������� ����
			strcpy(SMSToSend.SMSText, "Snyato!  ");
			if (1) { // ���� � ���������� ����� "������������ �����������", �� � ������ ��� ��������� ��������� ��������
				MakeTerm(tmpStr2);
				strcat(SMSToSend.SMSText, tmpStr2);
			}
			//swSMS = cswStart;
			swWork = cswWork;
		break;

		case cswWork:
			SetIndBits(0b00000000);
			if (flWasPrivUserCall != 0) {
				if (DIns == 0) {
					swWork = cswStartWarta;
				}
				else {
					SMSToSend.SMSPriv = cPrivilegeFull|cPrivilegeAllSMS;
					SMSToSend.SMSZones = DIns;
					strcpy(SMSToSend.SMSText, "Narushen kontur");
					if (DIns&0b00010000) strcat(SMSToSend.SMSText, " 1");
					if (DIns&0b00100000) strcat(SMSToSend.SMSText, " 2");
					if (DIns&0b01000000) strcat(SMSToSend.SMSText, " 3");
					if (DIns&0b10000000) strcat(SMSToSend.SMSText, " 4");
					strcat(SMSToSend.SMSText, " ");
					if (1) { // ���� � ���������� ����� "������������ �����������", �� � ������ ��� ��������� ��������� ��������
						MakeTerm(tmpStr2);
						strcat(SMSToSend.SMSText, tmpStr2);
					}
					//swSMS = cswStart;
				}
			}
		break;
		
// OHRANA
// � ���� ������:
// ���� �������� ������ - ������ ��� ����� 3 ������
// ���� ��������� ��� � ����� ������� �� ����������� ������� - ������� ����� �� "�����"
// ���� �������� ����� - ������� � ����� "�����"
		case cswStartWarta:
			CountRings = 0;
			//! ��������� �������� "��������� ���" � ������� "���� �� ������" �� ������ ������� ��������
			SMSToSend.SMSPriv = cPrivilegeFull|cPrivilegeAllSMS;
			SMSToSend.SMSZones = 0xFF; // ����������� ����
			strcpy(SMSToSend.SMSText, "Ohrana!  ");
			if (1) { // ���� � ���������� ����� "������������ �����������", �� � ������ ��� ��������� ��������� ��������
				MakeTerm(tmpStr2);
				strcat(SMSToSend.SMSText, tmpStr2);
			}
			//swSMS = cswStart;
			swWork = cswWarta;
		break;
		case cswWarta:
			SetIndBits(0b00000010);
			if (flWasPrivUserCall != 0) {
				swWork = cswStartWork;
			}
			if (DIns > 0) { // �������� �����
				swWork = cswStartAlarm;
				swAlarm = cswStart;
				CountRings=0;
			}
		break;
		
// KONTUR
// � ���� ������:
// ���� �������� ������ - ������ ��� ����� n ������
// ���� ��������� ��� � ����� ������� �� ����������� ������� - ������� ����� �� "�����" + ����� ������
		case cswStartKontur:
			CountRings = 0;
			if (DIns == 0) {
				swWork = cswStartWarta; // ����� ����� ��� ������!
			} 
			else {
				//! ��������� �������� "��������� ���" � ������� "������� ������" ������� ��������
				SMSToSend.SMSPriv = cPrivilegeFull|cPrivilegeAllSMS;
				SMSToSend.SMSZones = DIns;
				strcpy(SMSToSend.SMSText, "ALARM Narushen kontur ");
				if (DIns&0b00010000) strcat(SMSToSend.SMSText, " 1");
				if (DIns&0b00100000) strcat(SMSToSend.SMSText, " 2");
				if (DIns&0b01000000) strcat(SMSToSend.SMSText, " 3");
				if (DIns&0b10000000) strcat(SMSToSend.SMSText, " 4");
				strcat(SMSToSend.SMSText, " ");
				TPrevKontur = GetTime();
				if (1) { // ���� � ���������� ����� "������������ �����������", �� � ������ ��� ��������� ��������� ��������
					MakeTerm(tmpStr2);
					strcat(SMSToSend.SMSText, tmpStr2);
				}
				//swSMS = cswStart;
				swWork = cswKontur;
			}
		break;
		case cswKontur:
			// ����� ��������� ������ �� ���� ��� 20 ���, ���� ��� ������
			if ((GetTime() - TPrevKontur) >= Ticks(20000)) { 
				if (DIns == 0) {
					swWork = cswStartWarta; // ����� ����� ��� ������!
				}
				TPrevKontur = GetTime();
				break;
			}
			if (flWasPrivUserCall != 0) {
				swWork = cswStartWork; // 
				swCall = cswIdle;
			}
		break;
		
// ALARM
// � ���� ������:
// �������� ������
// ���� �������� ������ - ������ ��� ����� 1 ������
// ���� ��������� ��� � ����� ������� �� ����������� ������� - ������� ����� �� "�����" + ����� ������
// ������ 3 ������ (����� ����� �����������������) - ������� � ����� "������" + ����� ������
		case cswStartAlarm:
			CountRings = 0;
			//! ��������� �������� "��������� ���" � ������� "������" �� ������ ������� ��������
			SMSToSend.SMSPriv = cPrivilegeFull|cPrivilegeAllSMS|cPrivilegeAlarmSMS;
			SMSToSend.SMSZones = DIns;
			strcpy(SMSToSend.SMSText, "ALARM! ");
			if (DIns&0b00010000) strcat(SMSToSend.SMSText, " 1");
			if (DIns&0b00100000) strcat(SMSToSend.SMSText, " 2");
			if (DIns&0b01000000) strcat(SMSToSend.SMSText, " 3");
			if (DIns&0b10000000) strcat(SMSToSend.SMSText, " 4");
			strcat(SMSToSend.SMSText, " ");
			if (1) { // ���� � ���������� ����� "������������ �����������", �� � ������ ��� ��������� ��������� ��������
				MakeTerm(tmpStr2);
				strcat(SMSToSend.SMSText, tmpStr2);
			}
			//swSMS = cswStart;
			//swCall = cswStart; //! ������ �� ����������� �������
			swWork = cswAlarm;
		break;
		case cswAlarm:
			SetIndBits(0b00000011);
			if (flWasPrivUserCall != 0) {
				SirenaOff();
				swAlarm = cswIdle; //
				swWork = cswStartKontur; // ���� ���������� �������, ���� ����� ��� ������
				swCall = cswIdle;
			}
			
			// ������. �����������: ��� �� ������� ��� ��� ������ �� ������������ ��������
			switch (swAlarm) {
				case cswIdle:
				break;
		
				case cswStart:
					TPrevWorkAlarm = GetTime();
					SirenaOn();
					swAlarm = csw010;
				break;

				case csw010:
					AlarmTime_ms = Settings.AlarmTime_s;
					AlarmTime_ms = AlarmTime_ms * 1000;
					if ((GetTime() - TPrevWorkAlarm) >= Ticks(AlarmTime_ms)) { // 60 sec
						SirenaOff();
						swAlarm = cswIdle; //
						swWork = cswStartKontur; // ���� ���������� �������, ���� ����� ��� ������
						swCall = cswIdle;
					}
				break;
			}
			// END ������. �����������: ��� �� ������� ��� ��� ������ �� ������������ ��������
		break;
	}
	SendSMS();

	if (DIns) SrabotkaOn();
	else SrabotkaOff();
	
	flWasPrivUserCall = 0;

	return 0;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

TTime TMainAlg;
int swMainAlg = cswIdle;
char BadCount = 0;
char waitReadyBadCount = 0;
char flCallReady = 0;
int baudIndex = 0;
char MainAlg(void) {
	char tmpRet;
	char RetVal = 0;
	long int currentBaudRate;
	//char commandBaudRate[100];
	char baudRateStr[100];
	/*
	// �� ����� Call Ready ������ � ����� �����, � �� ������ ����� ����������������� ������!
	if (!flCallReady) {
		if (flIsReaded != 0) {
			if (strstr(FromModemBuf, "Call Ready")) {
				flCallReady = 1;
			}
		}
	}*/
	
	switch (swMainAlg) {
		case cswIdle:
		break;
		
		case cswStart:
			CurrentSet = 0;
			swMainAlg = cswStartModemConnectionSpeed;
			//swMainAlg = cswStartInitModem;
		break;
		
		case cswStartModemConnectionSpeed:
			if (baudIndex >= sizeOfBaudRates) {
				baudIndex = 0;
			}
			
			currentBaudRate = baudRates[baudIndex];
			USART0_Int_Init(BAUD(currentBaudRate));
			sprintf(baudRateStr, "%ld", currentBaudRate);
			strcat(baudRateStr, "\r\n");
			PutPC(baudRateStr);
			baudIndex++;
			
			MarkDelay(&TMainAlg, &swMainAlg, cswModemConnectionSpeed01);
		break;
		
		case cswModemConnectionSpeed01:
			WaitDelayAndGoTo(TMainAlg, 500, &swMainAlg, cswModemConnectionSpeed02);
		break;
		
		case cswModemConnectionSpeed02:
			DoCommand(&TMainAlg, "AT\r\n", &swMainAlg, cswModemConnectionSpeed03);
		break;
		
		case cswModemConnectionSpeed03:
			WaitAnswer(TMainAlg, "OK", 1000, &swMainAlg, cswStartInitModem, cswStartModemConnectionSpeed);
		break;
		
		case cswStartInitModem:
			if (CurrentSet >= cFirstSettsCount) {
				SetIndBits(0); //OutInd();
				swMainAlg = cswWaitReady;
			}
			else {
				DoCommand(&TMainAlg, FirstSetts[CurrentSet].CMDName, &swMainAlg, cswInitModem);
			}
		break;
		
		case cswInitModem:
			//if (WaitAnswer(TPrevWriteSettings, "OK", 2000, &swMainAlg, cswStartInitModem, cswStartInitModem)) CurrentSet++;
			if (WaitAnswer(TMainAlg, "OK", 2000, &swMainAlg, cswStartInitModem, cswStartInitModem)) 
				CurrentSet++;
		break;
		
		//------------ ----------------------------------------------------------
		//------------ ���� ���������� ������ -------------------------------
		/*
		case cswWaitReady:
			TMainAlg = GetTime();
			swMainAlg = csw010;
			swIndik = cswWaitReady;
		break;
		case csw010:
			if ((GetTime() - TMainAlg) >= Ticks(60000)) {
				swMainAlg = cswERRORWaitReady;
				swIndik = cswERRORWaitReady;
				break;
			}
			if (flIsReaded != 0) {
				//if (strstr(FromModemBuf, "Call Ready")) {
				if (flCallReady) {
					swMainAlg = cswOKWaitReady;
					break;
				}
			}
		break;
		case cswERRORWaitReady:
			TMainAlg = GetTime();
			BadCount++;
			if (BadCount >= 3) 
				RetVal = 100;
			swMainAlg = csw010;
		break;
		*/
		case cswWaitReady:
			DoCommand(&TMainAlg, "at+creg?\r\n", &swMainAlg, csw010);
			swIndik = cswWaitReady;
		break;
		case csw010:
			if (WaitAnswer(TMainAlg, "+CREG: 1,1", 2000, &swMainAlg, cswOKWaitReady, cswWaitReady)) {
				waitReadyBadCount++;
			}
			
			if (waitReadyBadCount > 8) {
				swMainAlg = cswERRORWaitReady;
				swIndik = cswERRORWaitReady;
			}
		break;
		case cswERRORWaitReady:
			RetVal = 100;
		break;
		case cswOKWaitReady:
			swMainAlg = cswReadSettings;
		break;
		//------------ END ���� ���������� ������ -------------------------------
		//------------ ----------------------------------------------------------
		
		//------------ ----------------------------------------------------------
		//------------ ������ ������ � ��������� �� ���������� ����� ------------
		case cswReadSettings:
			swReadSettings = cswStart;
			swMainAlg = csw020;
			swIndik = cswReadSettings;
		break;
		case csw020:
			tmpRet = ReadSettings();
			if ((tmpRet!=0) && (swReadSettings==cswNOK)) { // ������ ��������
				swReadSettings=cswIdle;
				swMainAlg = cswWriteSettings;
				PutPC("cswWriteSettings\r\n");
			}
			if ((tmpRet==0) && (swReadSettings==cswOK)) { // ���������� ���������
				swReadSettings=cswIdle;
				swMainAlg = cswStartWork;
				// swMainAlg = cswStartBilling;
			}
		break;
		//------------ END ������ ������ � ��������� �� ���������� ����� --------
		//------------ ----------------------------------------------------------

		//------------ ----------------------------------------------------------
		//------------ ���������� ������ � ���������
		case cswWriteSettings:
			swWriteSettings = cswStart;
			swMainAlg = csw040;
		break;
		case csw040:
			tmpRet = WriteSettings();
			if ((tmpRet!=0) && (swWriteSettings==cswNOK)) { // ������ ��������
				ModemOnOff();
				swWriteSettings=cswIdle;
				swMainAlg = cswERRORWriteSettings;
			}
			if ((tmpRet==0) && (swWriteSettings==cswOK)) { // ���������� ���������
				ModemOnOff();
				swWriteSettings=cswIdle;
				swMainAlg = cswENDWriteSettings;
			}
		break;
		case cswERRORWriteSettings:
		break;
		case cswENDWriteSettings:
		break;
		//------------ END ���������� ������ � ��������� � ���������� ����� -----
		//------------ ----------------------------------------------------------
		
		//------------ ----------------------------------------------------------
		//------------ ��������� ���� -------------------------------------------
		case cswStartBilling:
			swTryBilling = cswStart;
			swMainAlg = cswBilling;
		break;
		case cswBilling:
			TryBilling();
			switch (swTryBilling) {
				case cswCantConnectBill:
					if (IsDebug()) { //!#ifdef M_DEBUG
						PutPC("cswCantConnectBill\r\n");
					} //#endif
					swTryBilling = cswIdle;
					swMainAlg = cswStartWork;
				break;
				case cswLittleMoney:
					if (IsDebug()) { //!#ifdef M_DEBUG
						PutPC("cswLittleMoney\r\n");
					} //#endif
					flMoreThenMax = 0;
					swTryBilling = cswIdle;
					swMainAlg = cswStartWork;
					//!!!!!�������� ��� ���� ����� � ��������� ��� ����������
				break;
				case cswOK:
					if (IsDebug()) { //!#ifdef M_DEBUG
						PutPC("cswOK\r\n");
					} //#endif
					flMoreThenMax = 1;
					swTryBilling = cswIdle;
					swMainAlg = cswStartWork;
				break;
			}
		break;
		//------------ END ��������� ���� ---------------------------------------
		//------------ ----------------------------------------------------------
		
		//------------ ----------------------------------------------------------
		//------------ ��� ������ ���� - ����� ������������ �������� ------------
		case cswStartWork:
			swWork = cswWork;
			swMainAlg = cswWork;
			swIndik = cswStartWork;
			swAlgTermSirena = cswIdle;
		break;
		case cswWork:
			Work();
		break;
		//------------ END ��� ������ ���� - ����� ������������ �������� --------
		//------------ ----------------------------------------------------------
	}
	
	Indik();
	
	return RetVal;
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
