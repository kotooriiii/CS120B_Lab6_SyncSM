/*	Author: Carlos Miranda cmira039@ucr.edu
 *  Partner(s) Name: n/a
 *	Lab Section: 23
 *	Assignment: Lab #6  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 */
 
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

enum LightState {INIT, ACTION} LIGHT_STATE;

unsigned char LIGHT_CODE[3] =
{
	0x01,0x02,0x04
};

unsigned char currIndex = 0x00;
unsigned char lightCodeLen = 0x03;

unsigned char IPINA()
{
	return ~PINA;
}

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

void tickCounter()
{
	switch(LIGHT_STATE)
	{
		case INIT:
		LIGHT_STATE = ACTION;
		break;

		case ACTION:
		LIGHT_STATE = ACTION;
		break;
	}
	
	switch(LIGHT_STATE)
	{
		case INIT:
		PORTB = 0x00;
		currIndex = 0x00;
		break;
		
		case ACTION:
		if(currIndex < lightCodeLen)
		{
			PORTB = LIGHT_CODE[currIndex];
			currIndex++;
		
			if(currIndex == lightCodeLen)
			{
				currIndex = 0x00;
			}
		
		}
		break;
	}
}

int main(void) 
{
	
	//Outputs
	DDRB = 0xFF; PORTB = 0x00; 
	
	LIGHT_STATE = INIT;
	currIndex = 0x00;
	
	TimerSet(1000);
	TimerOn();
		
	while(1) 
	{
		tickCounter();
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;
}
