/*
 * Timer-ascention.c
 *
 * Created: 10/01/2023 16:26:39
 * Author : Fix-5
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

uint16_t val = 0;
uint16_t flag = 0;

void Init_Timer(void)
{
	TCCR1A |= ((1 << COM1A1) | (1 << WGM11));
	
	TCCR1B |= ((1 << WGM13) | (1 << WGM12) | (1 << CS10));
	
	TIMSK1 |= (1 << TOIE1);
	
	ICR1 = 3500;
	OCR1A = 0;
	
	DDRB |= (1<<PINB1);
}

ISR(TIMER1_OVF_vect)
{
	if (val < 3500 && flag == 0)
	{
		val++;
		OCR1A = val;
	}
	else if (val > 0 && flag == 1)
	{
		val--;
		OCR1A = val;
	}
		
	if (val == 3500)
	{
		flag = 1;
	}
	if(val == 0)
	{
		flag = 0;
	}
}


int main(void)
{
	Init_Timer();
	sei();
	
	while (1)
	{
		;
	}
}
