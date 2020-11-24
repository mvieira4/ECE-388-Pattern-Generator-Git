/*
 * Pattern Generator.c
 *
 * Created: 10/30/2020 12:13:11 PM
 * Author : pphantom
 */ 

#define  F_CPU 16000000
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>

int rotorCheck();

static uint8_t temp = 0;
static uint8_t i = 0;
static uint8_t turned = 0;
static uint8_t clockwise = 0;
static uint8_t mode = 0;
static uint8_t buttonPress = 0;
static uint8_t bit = 0;
static uint8_t channelNum = 0;
static uint8_t voltage = 0;
static uint8_t channel[8] = {0x65,0b01010101,0b00111011,0xf6,0x82,0x00,0x34,0b01010101};

int main(void)
{
	cli();

	DDRB = 0b11111110;
	PORTB = 0b11111111;
	
	DDRC = 0b00000000;
	PORTC = 0b11111111;

	DDRD = 0b01111111;
	PORTD = 0b01111111;
 
	DDRE = 0b11111111;
	PORTE = 0b11111110;

 	PCICR = 0b00000111;
 	PCMSK0 = 0b00000001;
 	PCMSK1 = 0b00000001;
	PCMSK2 = 0b10000000;

    OCR1A = 0x00FF;
    TCCR1B = (1 << WGM12);
    TIMSK1 = (1 << OCIE1A);
    TCCR1B = (0b01 << CS10);
	
	sei();
	
    while (1) 
    {	
		
		switch (mode)
		{
			case 0:
				channel[channelNum] = (channel[channelNum] + rotorCheck()) & 0b00111111;
				PORTD=~(channel[channelNum]);
				break;

			case 1:
				channelNum = (channelNum + rotorCheck()) % 8;
				PORTD=~(channelNum);
				break;

			case  2:
				voltage = (voltage + rotorCheck()) % 2;
				if (voltage == 0)
				{
					PORTD = 0b11101111;
				}
				else
				{
					PORTD = 0b11011111;
				}
				break;
		}
	}
}



int rotorCheck()
{
	if(turned)
	{
		turned = 0;
		if(clockwise)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	
	return 0;
}



ISR(PCINT0_vect)
{	
	if(!turned)
	{
	clockwise = 1;
	}
	turned = 1;
	_delay_ms(150);
}



ISR(PCINT1_vect)
{
	if(!turned)
	{
	clockwise = 0;
	}
	turned = 1;
	_delay_ms(150);
}



ISR(PCINT2_vect)
{
	if (!buttonPress)
	{
	mode = (mode + 1) % 3;
	}
	buttonPress = ~buttonPress;
}



ISR(TIMER1_COMPA_vect)
{	
	cli();
	
 	i=0;
 	temp = 0;
 	
 	for (i; i < 8; i++)
 	{
 		temp |= (((channel[i] >> (bit)) & 0b00000001) << i);
 	}
 	PORTB = temp & 0b11111110;
 	bit = (bit + 1) % 8;
	 
	sei();
}



