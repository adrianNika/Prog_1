/* GccApplication1.c
*
* Created: 11/01/2023 13:43:31
* Author : Fix-5
*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint-gcc.h>
#include <util/delay.h>
#include "AVR_Uart.h"


uint16_t actualTopValue = 0xFFFF;
uint16_t firstCapture = 0;
uint16_t secondCapture = 0;
uint8_t flag = 0;

uint8_t overflowWhileMeasurement = 0;
uint8_t computeNewTimeBase = 0;


ISR(TIMER1_OVF_vect)
{
	if (overflowWhileMeasurement == 0)
	{
		if (firstCapture != 0x0000)
		{
			overflowWhileMeasurement++;
		}
		else
		{
			/**/
		}
	}
}

ISR(TIMER1_CAPT_vect)
{	
	cli();
	
	secondCapture = ICR1;
	computeNewTimeBase = 1;

	_delay_us(200);
	
	TIFR1 |= (1 << ICF1);
	sei();
}

void Init_Timer (void)
{
	OCR1A = actualTopValue;
	
	TCCR1A = 0x00;
	TCCR1A |= (1 << COM1A0);
	
	TCCR1B = 0x00;
	TCCR1B |= 0b11001000;

	TIMSK1 = 0x00;
	TIMSK1 |= ((1 << TOIE1) | (1 << ICIE1));
}

void Start_timer (void)
{
	TCCR1B |= 0b11001101;
}

int main(void)
{
	DDRB |= 0x02;
	DDRD = 0x01;

	Init_Timer();
	UART_Init();

	sei();

	while (1)
	{
		PORTD = 0x01;
		_delay_ms(1000);
		PORTD = 0x00;
		
		if (PORTD == 0x00 && flag == 0)
		{
			Start_timer();
			firstCapture = ICR1;
			while(bit_is_clear(PINB,0));
			flag = 1;
		}
		
		if(computeNewTimeBase != 0)
		{
			cli();

			if (overflowWhileMeasurement <= 1)
			{
				if (secondCapture > firstCapture)
				{
					actualTopValue = secondCapture - firstCapture;
				}
				else
				{
					actualTopValue = (0xFFFF - firstCapture) + secondCapture;
				}
			}
			
			OCR1A = actualTopValue;
			TCNT1 = 0x0000;
			
			firstCapture = 0;
			secondCapture = 0;
			computeNewTimeBase = 0;
			overflowWhileMeasurement = 0;
			
			sei();
		}
	}
}