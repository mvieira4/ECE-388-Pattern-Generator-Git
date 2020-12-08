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
static uint8_t channel[8] = {0b11110000,0b11001100,0b10101010,0b11001111,0b11001111,0x22,0x34,0x22};

int main(void)
{
	cli();

	DDRB = 0b11111110;
	PORTB = 0b11111110;
	
	DDRC = 0b11111110;
	PORTC = 0b11111110;

	DDRD = 0b01111111;
	PORTD = 0b01111111;
 
	DDRE = 0b11111111;
	PORTE = 0b11111111;

 	PCICR = 0b00000111;
 	PCMSK0 = 0b00000001;
 	PCMSK1 = 0b00000001;
	PCMSK2 = 0b10000000;

    OCR1A = 0xFF00;
    TCCR1B = (1 << WGM12);
    TIMSK1 = (1 << OCIE1A);
    TCCR1B = (0b11 << CS10);
	
	sei();
	
	_delay_ms(1000);
	
    while (1) 
    {	
		
		switch (mode)
		{
			case  0:
				voltage = (voltage + rotorCheck()) % 2;
				if (voltage == 0)
				{
					PORTB = 0b11100001;
					PORTE = 0b11111110;
					_delay_ms(250);
					PORTB=0xFF;
					PORTE=0xFF;
					_delay_ms(250);
					PORTD&=0b10111111;
				}
				else
				{
					PORTB = 0b11110001;
					PORTE = 0b11111111;
					_delay_ms(250);
					PORTB=0xFF;
					PORTE=0xFF;
					_delay_ms(250);
					PORTD&=0b01000000;
				}
				break;
			
			case 1:
				channel[channelNum] = (channel[channelNum] + rotorCheck()) & 0b11111111;
				PORTB=~(channel[channelNum])<<1;
				PORTE=~(channel[channelNum])>>4;
				break;

			case 2:
				channelNum = (channelNum + rotorCheck()) % 8;
				PORTB=~(1<<channelNum)<<1;
				PORTE=~(1<<channelNum)>>4;
				_delay_ms(250);
				PORTB=0xFF;
				PORTE=0xFF;
				_delay_ms(250);
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
	_delay_ms(100);
}

ISR(PCINT1_vect)
{
	if (!buttonPress)
	{
	mode = (mode + 1) % 3;
	}
	buttonPress = ~buttonPress;
}

ISR(PCINT2_vect)
{
	if(!turned)
	{
	clockwise = 0;
	}
	turned = 1;
	_delay_ms(100);
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
 	PORTD = ~(temp & 0b00111111);
 	bit = (bit + 1) % 8;
	 
	sei();
}



