/*
 * test.c
 *
 * Created: 11/01/2023 14:40:50
 * Author : Fix-5
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>



int main(void)
{
    DDRC = 0x00;
	DDRD = 0xF0;
    while (1) 
    {
		if (PINC & (1 << PINC0))
		{
			PORTD = 0x20;
		} 
		else
		{
			PORTD = 0x00;
		}
    }
}

