/*
 * Test_sleepMode.c
 *
 * Created: 04/04/2023 10:48:42
 * Author : Fix-5
 */ 


#define F_CPU 16000000UL
#include <avr/io.h>
#include "AVR_Uart.h"
#include <util/delay.h>
#include <stdio.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdint-gcc.h>


int main(void)
{	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	
	sei();
	
    /* Replace with your application code */
    while (1) 
    {
		_delay_ms(1000);
		
		sleep_enable();
		sei();
		sleep_cpu();
    }
}



/*
ISR(INT1_vect)
{
	Uart_PutString("Laisse moi dormir!!!*");
	_delay_ms(1000);
	
	EIFR = 0;
}

int main(void)
{
	EICRA |= (1 << ISC10); // Interruption déclenchée sur front montant
	EIMSK |= (1 << INT1);  // Activation de l'interruption externe 1
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	
	UART_Init();
	sei();
	
	/ * Replace with your application code * /
	while (1)
	{
		_delay_ms(1000);
		Uart_PutString("je vais faire DODO*");
		_delay_ms(100);
		
		sleep_enable();
		sei();
		sleep_cpu();
	}
}*/