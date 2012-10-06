#ifndef UFUNCTION__H
#define UFUNCTION__H

#include "UGlobals.h"
#include "UTime.h"
#include "UAlgs.h"



#include "avr\io.h"
#include <avr\iom128.h>
#include <string.h>
#include <setjmp.h>
#include <util/delay.h>
#include <avr/interrupt.h>



void USART0_Init(unsigned int baud);
void USART0_Int_Init(unsigned int baud);
void USART1_Init(unsigned int baud);
void USART0_Transmit(char data);
void USART1_Transmit(char data);
void PutModemChar(char ValChar);
void PutPCChar(char ValChar);
void PutModem(char *str);
void PutPC(char *str);
void PutPCChars(char *str);
void PutPCCharsN(char *str, int Count);
char USART0_Receive(char *data);
char USART1_Receive(char *data);
void Wait(unsigned int n);
void MDelay(int ms);
void InitInsOuts(void);
void ModemOnOff(void);
void DIn(void);
void DOut(void);
void OutInd(void);
void OnIndBit(unsigned char OnBitState);
void OffIndBit(unsigned char OffBitState);
void SetIndBits(unsigned char NewState);
char ProcessKey(char Key);
char ReadStringModem(void);
char IsNewSMS(char *Str);
char IsNewOK(char *Str);
//-------------------------------------------

char DoCommand(TTime *DoCommandTimer, char *DoCommandCmd, int *swAlgDoCommand, int ValSwOK);
char WaitAnswer(TTime DoCommandTimer, char *Answer, TTime TimeOut, int *swAlgDoCommand, int ValSwOK, int ValSwNoOK);
//-------------------------------------------

void MarkDelay(TTime *pDoCommandTimer, int *pswAlgDoCommand, int ValSwOK);
void WaitDelayAndGoTo(TTime DoCommandTimer, TTime TimeOut, int *pswAlgDoCommand, int ValSwOK);

//-------------------------------------------
char IsDebug(void);

#endif
