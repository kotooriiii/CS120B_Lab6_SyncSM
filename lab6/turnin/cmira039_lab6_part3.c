/*	Author: Carlos Miranda cmira039@ucr.edu
 *  Partner(s) Name: n/a
 *	Lab Section: 23
 *	Assignment: Lab #6  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: 
 */
 
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum CounterType { INIT, WAIT_MAIN, INC, DEC, RESET, WAIT_INC, WAIT_DEC, WAIT_RESET} COUNTER_TYPE;
unsigned char TEMPC = 0x00;
unsigned char waitCounter = 0x00;
unsigned const char maxWaitCounter = 0x0A;


unsigned char IPINA()
{
	return ~PINA;
}

unsigned char isA0()
{
	return IPINA() & 0x01;
}

unsigned char isA1()
{
	return IPINA() & 0x02;
}


/*

	Time start

*/

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerISR()
{
	TimerFlag = 1;
}

void TimerOff()
{
	TCCR1B = 0x00;
}

void TimerOn()
{
	TCCR1B = 0x0B;

	OCR1A = 125;

	TIMSK1 = 0x02;

	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;

}

ISR(TIMER1_COMPA_vect)
{
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0)
	{
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M)
{
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

/*
	Time end
*/


void tickCounter()
{
	switch(COUNTER_TYPE)
	{
		case INIT:
		COUNTER_TYPE = WAIT_MAIN;
		break;
		
		case WAIT_MAIN:
		if(!isA0() && !isA1())
		{
			COUNTER_TYPE = WAIT_MAIN;
		}
		else if(isA0() && !isA1())
		{
			COUNTER_TYPE = INC;
		}
		else if(!isA0() && isA1())
		{
			COUNTER_TYPE = DEC;
		}
		else if(isA0() && isA1()){
			COUNTER_TYPE = RESET;
		}
		break;
		
		case INC:
		if(isA0() && isA1())
		{
			COUNTER_TYPE = RESET;
			break;
		} 
		COUNTER_TYPE = WAIT_INC;
		break;
		
		case WAIT_INC:
		if(isA0() && isA1())
		{
			COUNTER_TYPE = RESET;
		} 
		else if(isA0() && !isA1())
		{
			COUNTER_TYPE = WAIT_INC;
		}
		else{
			COUNTER_TYPE = WAIT_MAIN;
		}	
		break;
		
		case DEC:
		if(isA0() && isA1())
		{
			COUNTER_TYPE = RESET;
			break;
		} 
		COUNTER_TYPE = WAIT_DEC;
		break;

		case WAIT_DEC:
		if(isA0() && isA1())
		{
			COUNTER_TYPE = RESET;
		} 
		else if(!isA0() && isA1())
		{
			COUNTER_TYPE = WAIT_DEC;
		}
		else {
			COUNTER_TYPE = WAIT_MAIN;
		}
		break;
		
		case RESET:
		COUNTER_TYPE = WAIT_RESET;
		break;
		
		case WAIT_RESET:
		if(isA0() && isA1())
		{
			COUNTER_TYPE = WAIT_RESET;
		}
		else
		{
			COUNTER_TYPE = WAIT_MAIN;
		}
		break;
		
		default:
		COUNTER_TYPE = INIT;
		break;
	}
	
	switch(COUNTER_TYPE)
	{
		case INIT:
		waitCounter = 0x00;
		TEMPC = 0x07;
		PORTC = TEMPC;
		break;
		
		case WAIT_MAIN:
		waitCounter = 0x00;
		break;
		
		case INC:
		waitCounter = 0x00;
		if(TEMPC < 0x09)
		{
			TEMPC = TEMPC + 1;
			PORTC = TEMPC;
		}
		break;
		
		case WAIT_INC:
		if(waitCounter >= maxWaitCounter)
		{
			if(TEMPC < 0x09)
			{
				TEMPC = TEMPC + 1;
				PORTC = TEMPC;
			}
			
			waitCounter = 0x00;
		}
		else
		{
			waitCounter++;
		}
		
		break;
		
		case DEC:
		waitCounter = 0x00;
		if(TEMPC > 0x00)
		{
			TEMPC = TEMPC - 1;
			PORTC = TEMPC;
		}
		break;

		case WAIT_DEC:
		if(waitCounter >= maxWaitCounter)
		{
			if(TEMPC > 0x00)
			{
				TEMPC = TEMPC - 1;
				PORTC = TEMPC;
			}
			waitCounter = 0x00;
		}
		else
		{
			waitCounter++;
		}
		break;
		
		case RESET:
		TEMPC = 0x00;
		PORTC = TEMPC;
		waitCounter = 0x00;

		break;
		
		case WAIT_RESET:
		waitCounter = 0x00;
		break;
		
		default:
		break;
	}
	
}
	

int main(void) 
{
	
	//Inputs
	DDRA = 0x00; PORTA = 0xFF; 
	
	//Outputs
	DDRC = 0xFF; PORTC = 0x00; 
	
	COUNTER_TYPE = INIT;
	
	TEMPC = 0x07;
	PORTC = TEMPC;	
	
	TimerSet(100);
	TimerOn();
	
	while(1) 
	{
		tickCounter();
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 1;
}
	



