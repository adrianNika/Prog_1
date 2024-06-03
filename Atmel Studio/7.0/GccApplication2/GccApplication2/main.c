

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


int main(void)
{
	DDRC = 0x00;
	
	while(1)
	{
		PORTC = 0x01;
		_delay_ms(250);
		PORTC = 0x00;
		_delay_ms(750);
	}
}