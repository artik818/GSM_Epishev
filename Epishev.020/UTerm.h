#ifndef UTERM__H
#define UTERM__H

#include "avr\io.h"
#include <avr\iom128.h>
#include <string.h>
#include <setjmp.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#include "Const.h"
#include "UGlobals.h"
#include "UTime.h"


#define LOOP_CYCLES 8 //Number of cycles that the loop takes
#define us(num) (num/(LOOP_CYCLES*(1/(F_CPU/1000000.0))))


// Thermometer Connections (At your choice) 
/*
#define THERM_PORT PORTC
#define THERM_DDR DDRC
#define THERM_PIN PINC
#define THERM_DQ PC0
*/
#define THERM_PORT PORTF
#define THERM_DDR DDRF
#define THERM_PIN PINF
#define THERM_DQ0 PF0
#define THERM_DQ1 PF1


// Utils 
#define THERM_INPUT_MODE0() THERM_DDR&=~(1<<THERM_DQ0)
#define THERM_OUTPUT_MODE0() THERM_DDR|=(1<<THERM_DQ0)
#define THERM_LOW0() THERM_PORT&=~(1<<THERM_DQ0)
#define THERM_HIGH0() THERM_PORT|=(1<<THERM_DQ0)

#define THERM_INPUT_MODE1() THERM_DDR&=~(1<<THERM_DQ1)
#define THERM_OUTPUT_MODE1() THERM_DDR|=(1<<THERM_DQ1)
#define THERM_LOW1() THERM_PORT&=~(1<<THERM_DQ1)
#define THERM_HIGH1() THERM_PORT|=(1<<THERM_DQ1)

// A list of these commands translated into C defines:
#define THERM_CMD_CONVERTTEMP 0x44
#define THERM_CMD_RSCRATCHPAD 0xbe
#define THERM_CMD_WSCRATCHPAD 0x4e
#define THERM_CMD_CPYSCRATCHPAD 0x48
#define THERM_CMD_RECEEPROM 0xb8
#define THERM_CMD_RPWRSUPPLY 0xb4
#define THERM_CMD_SEARCHROM 0xf0
#define THERM_CMD_READROM 0x33
#define THERM_CMD_MATCHROM 0x55
#define THERM_CMD_SKIPROM 0xcc
#define THERM_CMD_ALARMSEARCH 0xec

#define THERM_DECIMAL_STEPS_12BIT 625 //.0625


union UTemper {
	uint8_t InByte[2];
	int16_t Value;
};

//extern union UTemper Temper;


#define THERM_COUNT 2 // кол-во датчиков температуры
#define STR_TERM_LEN 30
#define MAX_OTKAZ_COUNT 20
struct SOneTerm {
	union UTemper Temper;
	uint8_t Packet[9];
	TTime TTermAlg;
	int swTermAlg;
	int MINLim;
	int MAXLim;
	char flEnable; // выставляется при чтении конфигурации. если лимиты не установлены, то температура не меряется
	char flIsMINLim;
	char flIsMAXLim;
	char flOldIsMINLim;
	char flOldIsMAXLim;
	char trigMINLim;
	char trigMAXLim;
	char StrTherm[STR_TERM_LEN];
	char flOtkaz; 				// датчик на линии отсутствует
	char trigOtkaz; 			// только что отпал
	char OtkazCount; 			// кол-во попыток чтения датчика. если больше определенного, то выставляется флаг
	char OldOtkazCount;			// 
	char CRCOtkazCount;			// если что-то есть на линии, читаем данные, считаем СРС. если не совпала, увеличиваем счетчик
	char OldCRCOtkazCount;		// 
	char flCRCOtkaz;			// датчик плохо опрашивается или конкретные наводки
	char trigCRCOtkaz;			// только что перестала опрашиваться СРС.
//	char flGoodValue;			//
};
extern struct SOneTerm Terms[THERM_COUNT];



//------------------------------------------------------------------------------------

//extern inline __attribute__((gnu_inline)) void therm_delay(uint16_t delay);
//extern inline __attribute__((always_inline)) void therm_delay(uint16_t delay);

inline __attribute__((always_inline)) void therm_delay(uint16_t delay) {
	while (delay--) asm volatile("nop");
}

uint8_t therm_reset(char TermNum);
void therm_write_bit(uint8_t bit, char TermNum);
uint8_t therm_read_bit(char TermNum);
uint8_t therm_read_byte(char TermNum);
void therm_write_byte(uint8_t byte, char TermNum);
//void therm_read_temperature(char *buffer, char TermNum);
//------------------------------------------------------------------------------------

void TermInit (void);
char TermAlg(int TermNum);
void AlgTermSirena(void);
void MakeTerm(char *ResStr);

extern int swAlgTermSirena;
extern int otlTerm;
//------------------------------------------------------------------------------------
#define STR_TERM_LEN 30
//extern char StrTherm[STR_TERM_LEN];
//extern int swTermAlg;

//------------------------------------------------------------------------------------

#endif
