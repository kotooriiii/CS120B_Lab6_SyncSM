/*	Author: Carlos Miranda cmira039@ucr.edu
 *  Partner(s) Name: n/a
 *	Lab Section: 23
 *	Assignment: Lab #6  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/R0yl864lv1s
 */
 
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum LightState {INIT, ACTION, WAIT0, FREEZE, WAIT1} LIGHT_STATE;

unsigned char LIGHT_CODE[4] =
{
	0x01,0x02,0x04,0x02
};

unsigned char currIndex = 0x00;
unsigned char lightCodeLen = 0x04;

unsigned char IPINA()
{
	return ~PINA;
}

unsigned char isOnlyA0()
{
	return IPINA() & 0x01;
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
	switch(LIGHT_STATE)
	{
		case INIT:
		LIGHT_STATE = ACTION;
		break;

		case ACTION:
		if(isOnlyA0())
		{
			LIGHT_STATE = WAIT0;
		} 
		else
		{
			LIGHT_STATE = ACTION;
		}
		break;
		
		case WAIT0:
		if(isOnlyA0())
		{
			LIGHT_STATE = WAIT0;
		} 
		else
		{
			LIGHT_STATE = FREEZE;
		}
		break;
		
		case FREEZE:
		if(isOnlyA0())
		{
			LIGHT_STATE = WAIT1;
		} 
		else
		{
			LIGHT_STATE = FREEZE;
		}
		break;
		
		case WAIT1:
		if(isOnlyA0())
		{
			LIGHT_STATE = WAIT1;
		} 
		else
		{
			LIGHT_STATE = ACTION;
		}
		break;

	}
	
	switch(LIGHT_STATE)
	{
		case INIT:
		PORTB = 0x00;
		currIndex = 0x00;
		break;
		
		case WAIT0:
		break;
		
		case WAIT1:
		currIndex = 0x00;
		break;
		
		case FREEZE:
		//PORTB = LIGHT_CODE[currIndex];
		break;
		
		case ACTION:
		PORTB = LIGHT_CODE[currIndex];
		currIndex++;
	
		if(currIndex == lightCodeLen)
		{
			currIndex = 0x00;
		}
		
		
		break;
	}
}

int main(void) 
{
	
	//Inputs
	DDRA = 0x00; PORTA = 0xFF;
	//Outputs
	DDRB = 0xFF; PORTB = 0x00; 
	
	LIGHT_STATE = INIT;
	currIndex = 0x00;
	
	TimerSet(300);
	TimerOn();
		
	while(1) 
	{
		tickCounter();
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;
}
