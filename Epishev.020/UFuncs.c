#include <stdlib.h>
#include <stdio.h>

#include "UFuncs.h"



//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
ISR(USART0_RX_vect) {
	// Получаем данные
	char IntResCharM = UDR0;
	// Тут мы их обрабатываем
	tmpFromModemBuf[tmpFromModemBufLen++] = IntResCharM; // прочитали символ
	// строку считаем готовой если: или 0х0А, или закончился буфер чтения
	if ((tmpFromModemBufLen >= (MAX_BUF-5)) || (tmpFromModemBuf[tmpFromModemBufLen-1]==0x0A)) {
		if (tmpFromModemBufLen > 3) { // незачем нам пустые строчки
			//tmpFromModemBufLen -= 1; // переопределили длину строки
			tmpFromModemBuf[tmpFromModemBufLen-2] = 0; // нам в строке не нужны "црлф" - насильно закончили строку
			if (tmpFromModemBuf[tmpFromModemBufLen-3] == 0x0D) tmpFromModemBuf[tmpFromModemBufLen-3] = 0; // нам в строке не нужны "црлф" - насильно закончили строку
			strncpy(PromFromModemBuf, tmpFromModemBuf, MAX_BUF-1);
			tmp_flIsReaded = 1;
		}
		tmpFromModemBufLen = 0;
	}
}
//------------------------------------------------------------------------------------


void USART0_Init(unsigned int baud) {
	// Установка скорости связи 
	UBRR0H = (unsigned char)(baud>>8);
	UBRR0L = (unsigned char)baud;
	// Разрешение работы передатчика и приемника 
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	// Установка формата посылки: 8 бит данных, 1 стоп-бита 
	//UCSR0C = (3<<UCSZ0);
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);
	//UCSR0B=0xd8;
	//UCSR0C=0x86;
}
//------------------------------------------------------------------------------------

void USART0_Int_Init(unsigned int baud) {
	// Установка скорости связи 
	UBRR0H = (unsigned char)(baud>>8);
	UBRR0L = (unsigned char)baud;
	// Разрешение работы передатчика и приемника 
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	// Установка формата посылки: 8 бит данных, 1 стоп-бита 
	//UCSR0C = (3<<UCSZ0);
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);
	//Однако прерывание не будет вызываться, если микроконтроллеру этого не указать. 
	//В нашем случае для этого используется бит RXCIE в регистре UCSRB
	UCSR0B |= (1 << RXCIE0);
}


//------------------------------------------------------------------------------------


void USART1_Init(unsigned int baud) {
	// Установка скорости связи 
	UBRR1H = (unsigned char)(baud>>8);
	UBRR1L = (unsigned char)baud;
	// Разрешение работы передатчика и приемника 
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);
	// Установка формата посылки: 8 бит данных, 1 стоп-бита 
	//UCSR1C = (3<<UCSZ0);
	UCSR1C = (1<<UCSZ10)|(1<<UCSZ11);
	//UCSR1B=0xd8;
	//UCSR1C=0x86;
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------


// Передать байт в модем
void USART0_Transmit( char data ) {
	//Ожидание освобождения буфера передатчика
	while ( !( UCSR0A & (1<<UDRE0)) );
	//Помещение данных в буфер, отправка данных 
	UDR0 = data;
}
//------------------------------------------------------------------------------------

// Передать байт в РС
void USART1_Transmit( char data ) {
	//Ожидание освобождения буфера передатчика  
	while ( !( UCSR1A & (1<<UDRE1)) );
	//Помещение данных в буфер, отправка данных 
	UDR1 = data;
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

// Вывод символа в модем
void PutModemChar(char ValChar) {
	USART0_Transmit(ValChar);
}
//------------------------------------------------------------------------------------
// Вывод символа в PC
void PutPCChar(char ValChar) {
	USART1_Transmit(ValChar);
}
//------------------------------------------------------------------------------------

// Вывод строки в модем
void PutModem(char *str) {
	PutPC("To Modem->");
	PutPC(str);
	while (*str) {
		USART0_Transmit(*str);
		str++;
	}
}
//------------------------------------------------------------------------------------
// Вывод строки в PC
void PutPC(char *str) {
	while (*str) {
		USART1_Transmit(*str);
		str++;
	}
}

void PutPCChars(char *str) {
	char tmpCH1[3];
	while (*str) {
		sprintf(tmpCH1, "%02X", *str);
		USART1_Transmit(tmpCH1[0]);
		USART1_Transmit(tmpCH1[1]);
		str++;
	}
}
//------------------------------------------------------------------------------------

void PutPCCharsN(char *str, int Count) {
	char tmpCH1[3];
	for (int i_PutPCCharsN=0; i_PutPCCharsN<Count; ++i_PutPCCharsN) {
		sprintf(tmpCH1, "%02X", str[i_PutPCCharsN]);
		USART1_Transmit(tmpCH1[0]);
		USART1_Transmit(tmpCH1[1]);
		str++;
	}
}
//------------------------------------------------------------------------------------

// Получить байт с модема без ожидания 
//unsigned char USART0_Receive(unsigned char *data) {
char USART0_Receive(char *data) {
	//Ожидание окончания приема данных 
	if ( !(UCSR0A & (1<<RXC0)) ) { // нет данных в буфере чтения
		return 0;
	}else{
		//Загрузка принятых данных из буфера 
		*data = UDR0;
		return 1;
	}
	//return 0;
}
//------------------------------------------------------------------------------------

// Получить байт с PC без ожидания 
//unsigned char USART1_Receive(unsigned char *data) {
char USART1_Receive(char *data) {
	//Ожидание окончания приема данных 
	if ( !(UCSR1A & (1<<RXC1)) ) { // нет данных в буфере чтения
		return 0;
	}else{
		//Загрузка принятых данных из буфера 
		*data = UDR1;
		return 1;
	}
	//return 0;
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

unsigned int	i, j, k, l;
void Wait(unsigned int n) {
	for(i=0;i<n;i++){
		for(j=0;j<100;j++){
			for(k=0;k<65000;k++){
				l = k; l = l / 2;
			}
		}
	}
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

int CountTimes;
void MDelay(int ms) {
//	CountTimes = ms / 500;
//	for (i=0; i<CountTimes; i++) {
		//_delay_ms(10000);
		_delay_ms(ms);
//	}
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

void InitInsOuts(void) {
	//DDRG = 0xFF; // весь порт на вывод
	DDRG = 0b00011110; // младший бит на ввод  // для работы отладочного режима
	DDRD = 0xFF; // весь порт на вывод
	DDRE = 0x00; // весь порт на ввод
	DDRF = 0x00; // весь порт на ввод
	PORTG = 0b00000010;  // для работы отладочного режима 2-й бит всегда 1
	PORTD = 0xF0;
	PORTE = 0x00;
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

// Включение модема
/*
void ModemOn(void) {
//	PORTD = PORTD & 0b11111110;
//	MDelay(1000);
	PORTD = PORTD | 0x01;
	MDelay(1000);
	PORTD = PORTD & 0b11111110;
	MDelay(1000);
//	PORTD = PORTD | 0x01;
}
*/
//------------------------------------------------------------------------------------

// Выключение модема
void ModemOnOff(void) {
//	PORTD = PORTD & 0b11111110;
//	MDelay(500);
	//PORTD = PORTD | 0x01;
	DOuts = DOuts | 0x01;
	DOut();
	MDelay(1500);
	//PORTD = PORTD & (~0x01);
	DOuts = DOuts & (~0x01);
	DOut();
//	MDelay(1000);
//	PORTD = PORTD | 0x01;
//	PORTD = PORTD & 0b11111110;
}

//------------------------------------------------------------------------------------

void DIn(void) {
	//DIns = ~((PORTE>>4)&0b00001111);
	DIns = (~PINE)&0b11110000;
	DIns = DIns^Settings.DInInversion;
}
//------------------------------------------------------------------------------------
void DOut(void) {
	//PORTD = (PORTD&0b00001111) | (DOuts<<4);
	//PORTD = ((PORTD&0x0F) | (DOuts<<4));
	//PORTD = (PIND&0x0F) | ((~(DOuts<<4)) & 0b11110000);
	//PORTD = (PORTD&0x0F) | (DOuts&0xF0);
	PORTD = DOuts;
}
//------------------------------------------------------------------------------------

// реально вывод значений в физический порт
void OutInd(void) {
	//PORTG = ~(IndSate<<3);
	PORTG = ~((IndSate<<3)|1);
	//PORTG = PORTG | 0b00000010; // для работы отладочного режима 2-й бит всегда 1
	//PORTG&(~((0b00000011&IndSate)<<3));
}
//------------------------------------------------------------------------------------
//!!!! ТРЕБУЕТСЯ ПОМНИТЬ, ЧТО ИНДИКАТОРЫ ПОДЖИГАЮТСЯ НИЗКИМ УРОВНЕМ НА ДИСКРЕТНОМ ВЫХОДЕ !!!!
// производит установку соответствующих битов в 1.
// OnBitState принимает значение от 0b00 до 0b11 - в двоичной системе
// если какой-то разряд OnBitState == 0, то установка соответствующего бита не производится
void OnIndBit(unsigned char OnBitState) {
	IndSate = IndSate|(OnBitState);
}
// производит сброс соответствующих битов в 0.
// OnBitState принимает значение от 0b00 до 0b11 - в двоичной системе
// если какой-то разряд OnBitState == 0, то сброс соответствующего бита не производится
void OffIndBit(unsigned char OffBitState) {
	IndSate = IndSate&(~(OffBitState));
}
void SetIndBits(unsigned char NewState) {
	IndSate = NewState;
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
void PutNumsAndSets(void) {
	char tmpCh[MAX_BUF];
	char tmpStr[10];

		strncpy(tmpCh, "Setts: ", MAX_BUF-1);
		itoa(Settings.AlarmTime_s, tmpStr, 10);
		strncat(tmpCh, tmpStr,  MAX_BUF-1);
		strncat(tmpCh, ", ",  MAX_BUF-1);
		itoa(Settings.DInInversion, tmpStr, 10);
		strncat(tmpCh, tmpStr,  MAX_BUF-1);
		strncat(tmpCh, "\n\r",  MAX_BUF-1);
		PutPC(tmpCh);

	for (int CellNumber=cNumber01; CellNumber<cLast; ++CellNumber) {
		strncpy(tmpCh, PhoneNumber[CellNumber-cSettings].Number, MAX_BUF-1);
		strncat(tmpCh, ", ",  MAX_BUF-1);
		itoa(PhoneNumber[CellNumber-cSettings].IsActive, tmpStr, 10);
		strncat(tmpCh, tmpStr,  MAX_BUF-1);
		strncat(tmpCh, ", ",  MAX_BUF-1);
		itoa(PhoneNumber[CellNumber-cSettings].Privilege, tmpStr, 10);
		strncat(tmpCh, tmpStr,  MAX_BUF-1);
		strncat(tmpCh, ", ",  MAX_BUF-1);
		itoa(PhoneNumber[CellNumber-cSettings].Zones, tmpStr, 10);
		strncat(tmpCh, tmpStr,  MAX_BUF-1);
		strncat(tmpCh, ", ",  MAX_BUF-1);
		strncat(tmpCh, "\n\r",  MAX_BUF-1);
		PutPC(tmpCh);
	}

		strncpy(tmpCh, "MinMaxT1: ", MAX_BUF-1);
		itoa(Terms[0].MINLim, tmpStr, 10);
		strncat(tmpCh, tmpStr,  MAX_BUF-1);
		strncat(tmpCh, ", ",  MAX_BUF-1);
		itoa(Terms[0].MAXLim, tmpStr, 10);
		strncat(tmpCh, tmpStr,  MAX_BUF-1);
		strncat(tmpCh, "\n\r",  MAX_BUF-1);
		PutPC(tmpCh);
		strncpy(tmpCh, "MinMaxT2: ", MAX_BUF-1);
		itoa(Terms[1].MINLim, tmpStr, 10);
		strncat(tmpCh, tmpStr,  MAX_BUF-1);
		strncat(tmpCh, ", ",  MAX_BUF-1);
		itoa(Terms[1].MAXLim, tmpStr, 10);
		strncat(tmpCh, tmpStr,  MAX_BUF-1);
		strncat(tmpCh, "\n\r",  MAX_BUF-1);
		PutPC(tmpCh);

}

//------------------------------------------------------------------------------------
//unsigned char RetVal = 0;
char ProcessKey(char Key) {
	volatile char RetProcessKey = 0;
	switch (Key) {
		case '-': 
			PutNumsAndSets();
			//PutModem("at+cmgs=\"+380972116686\" \r\nasdfghjkl\x1A"); 
			//PutModemChar(26);
			//RetProcessKey = 1;
/*
					PutPC(">");
					PutPC(PhoneNumber[0].Number);
					PutPC("<");
					PutPC("\n\r");
					PutPC(">");
					PutPC(PhoneNumber[1].Number);
					PutPC("<");
					PutPC("\n\r");
					PutPC(">");
					PutPC(PhoneNumber[2].Number);
					PutPC("<");
					PutPC("\n\r");
*/
		break;
		
		case '6':
			otlTerm--;
		break;
		case '7':
			otlTerm++;
		break;
		
		case '*':
			PutPCChar(Terms[0].swTermAlg+0x30);
			PutPCChar(0x20);
			PutPCChar(Terms[1].swTermAlg+0x30);
			PutPC("\n\r");
		break;
		
		/*
		case '-': swReadSettings=cswStart; RetProcessKey = 1;
		break;
		case '-': PutModem("at+cusd=1,\"*101#\",15\r\n"); RetProcessKey = 1;
		break;
		case '-': RetVal = 0x27;
		break;
		case '1': PutModem("at"); RetProcessKey = 1;
		break;
		case '2': PutModem("at+cpin?"); RetProcessKey = 1;
		break;
		case '3': PutModem("at+ipr?"); RetProcessKey = 1;
		break;
		case '4': PutModem("at+"); RetProcessKey = 1;
		break;
		case '5': PutModem("at+ipr=115200"); RetProcessKey = 1;
		break;
		case '9': //ModemOn(); RetProcessKey = 1;
		break;
		*/
		case '`': ModemOnOff(); RetProcessKey = 1;
		break;
		case '1': 
			flIsReaded = 2;
		break;
	}
	return RetProcessKey;
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

//Чтение строки с модема. Читает до «CR»«LF» или до maxBuf.
//Чтение без ожидания
//В Str помещается прочитанная строка
//StrIndex - индекс следующей буквы в строке. фактически - количество букв в строке
//Возвращает: 
//  0 - строка наконец-то дочитана до "црлф", в случае ошибки - ее код.
//  1 - строка не дочитана - нет новых символов
//  2 - строка не дочитана - есть новые символы
// 10 - переполнение буфера, а строка до сих пор не прочитана

// flIsReaded выставляется в 1, когда строка прочитана из порта. 
// в 0 - когда строка или не была еще прочитана ни разу или пришел запрос "выставить флаг в 0"
// функция должна читать символы и формировать строку. если строка прочитана, то выставляется соответствующий флаг.
// как только строка дочитана, она перемещается в другой буфер, с которым может работать основной цикл программы


char ReadStringModem(void) {
	char RetValReadStringModem = 1; // ни один символ не прочитан
	if (USART0_Receive(&ResCharM)) { // получили символ из порта от модема
		USART1_Transmit(ResCharM); //!!!!! тут только для отладки !!!!!
		//USART1_Transmit("2"); //!skjdhaskdh
		if (tmpFromModemBufLen >= (MAX_BUF-5)) {
			RetValReadStringModem = 10; // переполнение буфера, а строка до сих пор не прочитана
			USART1_Transmit('9'); //!skjdhaskdh
			tmpFromModemBufLen = 0; // так себе заглушка
		}
		else {
			RetValReadStringModem = 2;
			tmpFromModemBuf[tmpFromModemBufLen] = ResCharM;
			if (tmpFromModemBufLen >= 1) {
				if ((tmpFromModemBuf[tmpFromModemBufLen-1]==0x0D) && (tmpFromModemBuf[tmpFromModemBufLen]==0x0A)) {
					
					tmpFromModemBufLen -= 1; // переопределили длину строки
					tmpFromModemBuf[tmpFromModemBufLen] = 0; // нам в строке не нужны "црлф" - насильно закончили строку
					//USART1_Transmit('5'); //!skjdhaskdh
					FromModemBufLen = tmpFromModemBufLen;
					strncpy(FromModemBuf, tmpFromModemBuf, FromModemBufLen+1);
					tmpFromModemBufLen = 0;
					flIsReaded = 1;
					RetValReadStringModem = 0; // наконец-то прочли строку до конца! 
					return RetValReadStringModem;
				}
			}
			tmpFromModemBufLen++;
		}
	}
	return RetValReadStringModem;
}

//------------------------------------------------------------------------------------

// Определение является ли строка Str сигналом о том, что пришло новое сообщение.
// +CMTI:"SM",
char IsNewSMS(char *Str) {
	//if (strlen(Str) < 12) return 0; // не может быть строка, содержащая СМС
	if (strstr(Str, "+CMTI:\"SM\"") == NULL) return 0; // нет такой буквы в этом слове :)
	return 1; // нашли вхождение!
}

char IsNewOK(char *Str) {
	//if (strlen(Str) < 2) return 0; // не может быть строка, содержащая СМС
	if (strstr(Str, "OK") == NULL) return 0; // нет такой буквы в этом слове :)
	return 1; // нашли вхождение!
}
//------------------------------------------------------------------------------------

char DoCommand(TTime *pDoCommandTimer, char *DoCommandCmd, int *pswAlgDoCommand, int ValSwOK) {
	PutModem(DoCommandCmd);
	*pDoCommandTimer = GetTime();
	*pswAlgDoCommand = ValSwOK;
	return 1;
}
char WaitAnswer(TTime DoCommandTimer, char *Answer, TTime TimeOut, int *pswAlgDoCommand, int ValSwOK, int ValSwNoOK) {
	char RetValWaitAnswer = 0; // еще работает
	if ((GetTime() - DoCommandTimer) >= Ticks(TimeOut)) {
		RetValWaitAnswer = 4; // время прошло, а ответа от модема нет!
		*pswAlgDoCommand = ValSwNoOK;
	} else
	if (flIsReaded != 0) {
		if (strstr(FromModemBuf, Answer)) {
			RetValWaitAnswer = 1; // все хорошо
			*pswAlgDoCommand = ValSwOK;
		}
	}
	return RetValWaitAnswer;
}
//------------------------------------------------------------------------------------

void MarkDelay(TTime *pDoCommandTimer, int *pswAlgDoCommand, int ValSwOK) {
	*pDoCommandTimer = GetTime();
	*pswAlgDoCommand = ValSwOK;
}

void WaitDelayAndGoTo(TTime DoCommandTimer, TTime TimeOut, int *pswAlgDoCommand, int ValSwOK) {
	if ((GetTime() - DoCommandTimer) >= Ticks(TimeOut)) {
		*pswAlgDoCommand = ValSwOK;
	}
}
//------------------------------------------------------------------------------------
char IsDebug(void) {
	//char tmpDIns = (~PINE)&0b11110000;
	//if (tmpDIns&0b00010000) return 1; // если включен дискретный вход
	return 1;
	char tmpDIns = PING&0x01;
	if (tmpDIns) return 1; // если включен дискретный вход
	else  return 0;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
