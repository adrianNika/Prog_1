/*
 * TEST_MOSFET.c
 *
 * Created: 11/05/2023 14:45:59
 * Author : Fix-5
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
	DDRD = 0x01;
    /* Replace with your application code */
    while (1) 
    {
		PORTD ^= 0x01;
		_delay_ms(1000);
    }
}

