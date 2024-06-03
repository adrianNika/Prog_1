/*
 * R2R_TEST.c
 *
 * Created: 24/04/2023 11:15:05
 * Author : Fix-5
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRC = 0x0F;
	DDRD = 0x0F;
    /* Replace with your application code */
    while (1) 
    {
		/*PORTC = PIND;*/
		
		for (int i = 0; i < 17; i++)
		{
			PORTC = i;
			_delay_ms(200);
		}
    }
}

