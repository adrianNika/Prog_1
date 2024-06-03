/*
 * Timer.c
 *
 * Created: 10/01/2023 13:34:41
 * Author : Fix-5
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>

void Init_Timer(void)
{
	TCCR1A |= ((1<<COM1A1) | (1<<WGM11));
	TCCR1B |= ((1<<WGM13) | (1<<WGM12) | (1<<CS10));
	
	ICR1 = 380;
	OCR1A = 159;
	
	DDRB |= (1<<PINB1);
}

int main(void)
{
    Init_Timer();
	
    while (1) 
    {
		;
    }
}

