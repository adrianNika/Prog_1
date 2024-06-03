/*
 * capteur ultra son.c
 *
 * Created: 07/08/2023 14:33:07
 * Author : Fix-5
 */ 

#include <avr/io.h>
#include "AVR_Uart.h"
#include "global.h"


int main(void)
{
	DDRC = 0x01;
    /* Replace with your application code */
    while (1) 
    {
		PORTC = 0x01;
		_delay_ms(500);
		PORTC = 0x00;
		_delay_ms(500);
    }
}

