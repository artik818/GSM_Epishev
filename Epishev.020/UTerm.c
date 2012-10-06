#include <stdlib.h>
#include <stdio.h>

#include "UTerm.h"
#include "UFuncs.h"

//------------------------------------------------------------------------------------

//char StrTherm[STR_TERM_LEN];

//union UTemper Temper;

struct SOneTerm Terms[THERM_COUNT];


//------------------------------------------------------------------------------------

uint8_t therm_reset(char TermNum) {
	uint8_t Ret;
	if (TermNum==0) {
		//Pull line low and wait for 480uS
		THERM_LOW0();
		THERM_OUTPUT_MODE0();
		therm_delay(us(480));
		//Release line and wait for 60uS
		THERM_INPUT_MODE0();
		therm_delay(us(60));
		//Store line value and wait until the completion of 480uS period
		Ret=(THERM_PIN & (1<<THERM_DQ0));
		therm_delay(us(420));
	}
	else {
		//Pull line low and wait for 480uS
		THERM_LOW1();
		THERM_OUTPUT_MODE1();
		therm_delay(us(480));
		//Release line and wait for 60uS
		THERM_INPUT_MODE1();
		therm_delay(us(60));
		//Store line value and wait until the completion of 480uS period
		Ret=(THERM_PIN & (1<<THERM_DQ1));
		therm_delay(us(420));
	}
	//Return the value read from the presence pulse (0=OK, 1=WRONG)
	return Ret;
}
//------------------------------------------------------------------------------------

void therm_write_bit(uint8_t bit, char TermNum) {
	if (TermNum==0) {
		//Pull line low for 1uS
		THERM_LOW0();
		THERM_OUTPUT_MODE0();
		therm_delay(us(1));
		//If we want to write 1, release the line (if not will keep low)
		if(bit) THERM_INPUT_MODE0();
		//Wait for 60uS and release the line
		therm_delay(us(60));
		THERM_INPUT_MODE0();
	}
	else {
		//Pull line low for 1uS
		THERM_LOW1();
		THERM_OUTPUT_MODE1();
		therm_delay(us(1));
		//If we want to write 1, release the line (if not will keep low)
		if(bit) THERM_INPUT_MODE1();
		//Wait for 60uS and release the line
		therm_delay(us(60));
		THERM_INPUT_MODE1();
	}
}
//------------------------------------------------------------------------------------

uint8_t therm_read_bit(char TermNum) {
	uint8_t bit=0;
	if (TermNum==0) {
		//Pull line low for 1uS
		THERM_LOW0();
		THERM_OUTPUT_MODE0();
		therm_delay(us(1));
		//Release line and wait for 14uS
		THERM_INPUT_MODE0();
		therm_delay(us(14));
		//Read line value
		if(THERM_PIN&(1<<THERM_DQ0)) bit=1;
		//Wait for 45uS to end and return read value
		therm_delay(us(45));
	}
	else {
		//Pull line low for 1uS
		THERM_LOW1();
		THERM_OUTPUT_MODE1();
		therm_delay(us(1));
		//Release line and wait for 14uS
		THERM_INPUT_MODE1();
		therm_delay(us(14));
		//Read line value
		if(THERM_PIN&(1<<THERM_DQ1)) bit=1;
		//Wait for 45uS to end and return read value
		therm_delay(us(45));
	}
	return bit;
}
//------------------------------------------------------------------------------------

uint8_t therm_read_byte(char TermNum) {
	uint8_t i=8, n=0;
	while(i--){
		//Shift one position right and store read value
		n>>=1;
		n|=(therm_read_bit(TermNum)<<7);
	}
	return n;
}
//------------------------------------------------------------------------------------

void therm_write_byte(uint8_t byte, char TermNum) {
	uint8_t i=8;
	while(i--){
		//Write actual bit and shift one position right to make
		//the next bit ready
		therm_write_bit(byte&1, TermNum);
		byte>>=1;
	}
}
//------------------------------------------------------------------------------------
/*
void therm_read_temperature(char *buffer, char TermNum) {
	// Buffer length must be at least 12bytes long! ["+XXX.XXXX C"]
	uint8_t temperature[2];
	int8_t digit;
	uint16_t decimal;
	//Reset, skip ROM and start temperature conversion
	therm_reset();
	therm_write_byte(THERM_CMD_SKIPROM);
	therm_write_byte(THERM_CMD_CONVERTTEMP);
	//Wait until conversion is complete
	while(!therm_read_bit());
	//Reset, skip ROM and send command to read Scratchpad
	therm_reset();
	therm_write_byte(THERM_CMD_SKIPROM);
	therm_write_byte(THERM_CMD_RSCRATCHPAD);
	//Read Scratchpad (only 2 first bytes)
	temperature[0]=therm_read_byte();
	temperature[1]=therm_read_byte();
	therm_reset();
	//Store temperature integer digits and decimal digits
	digit=temperature[0]>>4;
	digit|=(temperature[1]&0x7)<<4;
	//Store decimal digits
	decimal=temperature[0]&0xf;
	decimal*=THERM_DECIMAL_STEPS_12BIT;
	//Format temperature into a string [+XXX.XXXX C]
	//sprintf(buffer, "%+d.%04u C\r", digit, decimal);
	sprintf(buffer, "%d.%04u C\r", digit, decimal);
}
//------------------------------------------------------------------------------------
*/
//------------------------------------------------------------------------------------

// включить сирену термометра
void TermSirenaOn(void) {
	DOuts = DOuts & 0b10111111; 
}
//------------------------------------------------------------------------------------

// выключить сирену термометра
void TermSirenaOff(void) {
	DOuts = DOuts | 0b01000000; 
}
//------------------------------------------------------------------------------------

TTime TAlgSirena;
int swAlgTermSirena = cswIdle;
void AlgTermSirena(void) {
	switch (swAlgTermSirena) {
		case cswIdle:
		break;

		case cswStart:
			TAlgSirena = GetTime();
			TermSirenaOn();
			swAlgTermSirena = csw010;
		break;

		case csw010:
			if ((GetTime() - TAlgSirena) >= Ticks(15000)) { // 15 sec
				TermSirenaOff();
				swAlgTermSirena = cswIdle; //
			}
		break;
	}
}
//------------------------------------------------------------------------------------

void TermInit (void) {
	for (int i=0; i<THERM_COUNT; ++i) {
		Terms[i].swTermAlg = cswIdle;
		Terms[i].flIsMINLim = 0;
		Terms[i].flIsMAXLim = 0;
		Terms[i].flOldIsMINLim = 0;
		Terms[i].flOldIsMAXLim = 0;
		Terms[i].flOtkaz = 0;
		Terms[i].OtkazCount = 0;
		Terms[i].OldOtkazCount = 0;
		Terms[i].CRCOtkazCount = 0;
		Terms[i].OldCRCOtkazCount = 0;
		Terms[i].MINLim = 23;
		Terms[i].MAXLim = 25;
		Terms[i].trigMINLim = 0;
		Terms[i].trigMAXLim = 0;
		Terms[i].trigOtkaz = 0;
		Terms[i].trigCRCOtkaz = 0;
		Terms[i].flEnable = 1;
	}
}
//------------------------------------------------------------------------------------
#define CRC8INIT	0x00
#define CRC8POLY	0x18              //0X18 = X^8+X^5+X^4+X^0

uint8_t crc8( uint8_t *data_in, uint16_t number_of_bytes_to_read ) {
	uint8_t  crc;
	uint16_t loop_count;
	uint8_t  bit_counter;
	uint8_t  data;
	uint8_t  feedback_bit;
	
	crc = CRC8INIT;
	for (loop_count = 0; loop_count != number_of_bytes_to_read; loop_count++) {
		data = data_in[loop_count];
		bit_counter = 8;
		do {
			feedback_bit = (crc ^ data) & 0x01;
			if ( feedback_bit == 0x01 ) crc = crc ^ CRC8POLY;
			crc = (crc >> 1) & 0x7F;
			if ( feedback_bit == 0x01 ) crc = crc | 0x80;
			data = data >> 1;
			bit_counter--;
		} while (bit_counter > 0);
	}
	return crc;
}

//------------------------------------------------------------------------------------

//TTime TTermAlg;
//int swTermAlg = cswIdle;
int otlTerm = 23;
int Byte_index;
char tmpStr2[160];
char TermAlg(int TermNum) {
	
	char RetVal = 0;
	uint8_t ResCRC;
	
	switch (Terms[TermNum].swTermAlg) {
		case cswIdle:
		break;
		
		case cswStart:
			//Reset, skip ROM and start temperature conversion
			therm_reset(TermNum);
			therm_write_byte(THERM_CMD_SKIPROM, TermNum);
			therm_write_byte(THERM_CMD_CONVERTTEMP, TermNum);
			Terms[TermNum].TTermAlg = GetTime();
			Terms[TermNum].swTermAlg = csw010;
		break;
		
		case csw010:
			if (therm_read_bit(TermNum)) {
				Terms[TermNum].OldOtkazCount = 0;
				Terms[TermNum].OtkazCount = 0;
				Terms[TermNum].flOtkaz = 0;
				Terms[TermNum].swTermAlg = csw020;
			} 
			else {
				if ((GetTime() - Terms[TermNum].TTermAlg) > Ticks(100)) { // в течении 100 мс
					RetVal = 1; // не появился ответ на линии
					Terms[TermNum].swTermAlg = cswIdle;
					if (Terms[TermNum].OtkazCount >= MAX_OTKAZ_COUNT) {
						if (Terms[TermNum].OtkazCount != Terms[TermNum].OldOtkazCount) {
							Terms[TermNum].flOtkaz = 1;
							Terms[TermNum].trigOtkaz = 1;
							Terms[TermNum].flIsMINLim = 0; // не может быть минимального лимита при отказе датчика
							Terms[TermNum].flIsMAXLim = 0; // не может быть максимального лимита при отказе датчика
							strcpy(Terms[TermNum].StrTherm, "?");
						}
						Terms[TermNum].OldOtkazCount = Terms[TermNum].OtkazCount;
					}
					else {
						Terms[TermNum].OldOtkazCount = Terms[TermNum].OtkazCount;
						Terms[TermNum].OtkazCount++;
					}
				}
			}
		break;
		
		case csw020:
			therm_reset(TermNum);
			therm_write_byte(THERM_CMD_SKIPROM, TermNum);
			therm_write_byte(THERM_CMD_RSCRATCHPAD, TermNum);
			//Read Scratchpad (only 2 first bytes)
			Terms[TermNum].Temper.InByte[0] = therm_read_byte(TermNum); // младший байт
			Terms[TermNum].Temper.InByte[1] = therm_read_byte(TermNum); // старший байт
			Terms[TermNum].Packet[0] = Terms[TermNum].Temper.InByte[0];
			Terms[TermNum].Packet[1] = Terms[TermNum].Temper.InByte[1];
			for (int i=2; i<=8; ++i) Terms[TermNum].Packet[i] = therm_read_byte(TermNum);
			Terms[TermNum].swTermAlg = csw025;
			
		break;

		case csw025:
			ResCRC = crc8(Terms[TermNum].Packet, 8);
			if (ResCRC == Terms[TermNum].Packet[8]) { // crc совпала
				Terms[TermNum].OldCRCOtkazCount = 0;
				Terms[TermNum].CRCOtkazCount = 0;
				Terms[TermNum].flCRCOtkaz = 0;
				Terms[TermNum].swTermAlg = csw030;
			}
			else { // crc не совпала
				strcpy(Terms[TermNum].StrTherm, "?");
				RetVal = 2; // не совпала CRC
				Terms[TermNum].swTermAlg = cswIdle;
				if (Terms[TermNum].CRCOtkazCount >= MAX_OTKAZ_COUNT) {
					if (Terms[TermNum].CRCOtkazCount != Terms[TermNum].OldCRCOtkazCount) {
						Terms[TermNum].flCRCOtkaz = 1;
						Terms[TermNum].trigCRCOtkaz = 1;
						Terms[TermNum].flIsMINLim = 0; // не может быть минимального лимита при отказе датчика
						Terms[TermNum].flIsMAXLim = 0; // не может быть максимального лимита при отказе датчика
					}
					Terms[TermNum].OldCRCOtkazCount = Terms[TermNum].CRCOtkazCount;
				}
				else {
					Terms[TermNum].OldCRCOtkazCount = Terms[TermNum].CRCOtkazCount;
					Terms[TermNum].CRCOtkazCount++;
				}
			}
		break;

		case csw030:
			Terms[TermNum].Temper.Value = Terms[TermNum].Temper.Value >> 4;
			//Terms[TermNum].Temper.Value = otlTerm; //!!!!!
			Terms[TermNum].flOldIsMINLim = Terms[TermNum].flIsMINLim;
			Terms[TermNum].flOldIsMAXLim = Terms[TermNum].flIsMAXLim;
			if (Terms[TermNum].Temper.Value < Terms[TermNum].MINLim) Terms[TermNum].flIsMINLim = 1;
			else Terms[TermNum].flIsMINLim = 0;
			if (Terms[TermNum].Temper.Value > Terms[TermNum].MAXLim) Terms[TermNum].flIsMAXLim = 1;
			else Terms[TermNum].flIsMAXLim = 0;
			// если есть тригер по какой-либо границе - запускаем сирену
			if ((Terms[TermNum].flOldIsMINLim==0) && (Terms[TermNum].flIsMINLim==1)) Terms[TermNum].trigMINLim = 1;
			if ((Terms[TermNum].flOldIsMAXLim==0) && (Terms[TermNum].flIsMAXLim==1)) Terms[TermNum].trigMAXLim = 1;
			
			sprintf(Terms[TermNum].StrTherm, "%dC", Terms[TermNum].Temper.Value);
			Terms[TermNum].TTermAlg = GetTime();
			Terms[TermNum].swTermAlg = csw040;
		break;

		case csw040:
			if ((GetTime() - Terms[TermNum].TTermAlg) > Ticks(5000)) Terms[TermNum].swTermAlg = cswIdle; // 5 сек.
		break;
	}
	return RetVal;
}
//------------------------------------------------------------------------------------


void MakeTerm(char *ResStr) {
	char tmpStr2[160];
	char tmpStr3[10];
	//memset(tmpStr2, 0, 160);
	tmpStr2[0] = 0;
	for (int i=0; i<THERM_COUNT; ++i) {
		if (!Terms[i].flEnable) continue;
		strcat(tmpStr2, "t");
		itoa(i+1, tmpStr3, 10);
		strcat(tmpStr2, tmpStr3);
		strcat(tmpStr2, "=");
		if (Terms[i].flOtkaz == 1) strcat(tmpStr2, "ErrNON");
		else if (Terms[i].flCRCOtkaz == 1) strcat(tmpStr2, "ErrCRC");
		else strcat(tmpStr2, Terms[i].StrTherm);
		strcat(tmpStr2, "; "); 
		
		if (Terms[i].flIsMINLim==1) strcat(tmpStr2, "MIN!!!; ");
		if (Terms[i].flIsMAXLim==1) strcat(tmpStr2, "MAX!!!; ");
		
		strcat(tmpStr2, "#");
	}
	strcpy(ResStr, tmpStr2);
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
