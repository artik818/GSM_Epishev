
#include "UTime.h"


volatile unsigned int TimerVar5ms = 0;


//------------------------------------------------------------------------------------

TTime GetTime(void) {
	unsigned char sreg;
	unsigned int retVal;
	sreg = SREG;
	cli();
	retVal = TimerVar5ms;
	SREG = sreg;
	return retVal;
}
//------------------------------------------------------------------------------------

unsigned int Ticks(unsigned long int Time_ms) {
	return Time_ms / 5;
}
//------------------------------------------------------------------------------------
// прерывания
//13 $0018 TIMER1 COMPA Срабатывание компаратора А таймера-счетчика 1 
//14 $001A TIMER1 COMPB Срабатывание компаратора В таймера-счетчика 1 
//15 $001C TIMER1 OVF Переполнение таймера-счетчика 1

// Timer/Counter1 Compare Match A 
//#define TIMER1_COMPA_vect		_VECTOR(12)
//#define SIG_OUTPUT_COMPARE1A		_VECTOR(12)

//Разряд 0 – IVCE: Разрешение изменения вектора прерывания 

//------------------------------------------------------------------------------------
// делаем заглушку для всех векторов прерываний
char WasBadVect = 0;
ISR(BADISR_vect)
{
	WasBadVect = 1;
}
//------------------------------------------------------------------------------------

ISR(TIMER1_COMPA_vect) {
	//sei();
	TimerVar5ms++;
	TIFR = (1<<OCF1A); // выставить в 1 для осуществления прерывания!
}
//------------------------------------------------------------------------------------

void initTimer(void) {
	// OCR1A = 73728;
	TCNT1 = 0;
	OCR1A = 36864; // 5ms // задаем верхний предел
	TCCR1B = ((1<<WGM12) | (1<<CS10));
	TIMSK = (1<<OCIE1A); // Timer/Counter1 Compare Match interrupt is enabled
	TIFR = (1<<OCF1A); // выставить в 1 для осуществления прерывания!
}
//------------------------------------------------------------------------------------
//?INTERRUPT(TIMER1_COMPA_vect) {
//?ISTR(TIMER1_COMPA_vect) {
//Catch-all interrupt vector
//If an unexpected interrupt occurs (interrupt is enabled and 
//no handler is installed, which usually indicates a bug), 
//then the default action is to reset the device by jumping 
//to the reset vector. You can override this by supplying a 
//function named BADISR_vect which should be defined with ISR() as such. 
//(The name BADISR_vect is actually an alias for __vector_default. 
//The latter must be used inside assembly code in case <avr/interrupt.h> 
//is not included.)
//#include <avr/interrupt.h>
//ISR(BADISR_vect)
//{
//    // user code here
//}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
