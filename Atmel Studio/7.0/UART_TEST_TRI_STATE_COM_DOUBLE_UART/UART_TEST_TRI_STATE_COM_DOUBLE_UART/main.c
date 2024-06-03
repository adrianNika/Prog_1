/*
 * UART_TEST_TRI_STATE_COM_DOUBLE_UART.c
 *
 * Created: 06.02.2024 10:07:28
 * Author : adria
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "AVR_Uart.h"
#include <stdint.h>
#include <stdio.h>

int main(void)
{
	DDRD = 0x02;
	DDRC = 0x3F;
	sei();
	UART_Init();
    /* Replace with your application code */
    while (1) 
    {
		Uart_PutString("peripherique 1;",0);
		_delay_ms(100);
		Uart_PutString("peripherique 2;",1);
		while(1);
    }
}

