/*
 * main.c
 *
 * Created: 17.02.2021 10:38:30
 * Author : AppMan
 */ 

#define  F_CPU 16000000UL
#include "AVR_Uart.h"
#include "util/delay.h"

int main(void)
{	
	UART_Init();
	
	sei(); 

    while (1) 
    {
		Uart_PutString("HELLO WORLD!\r\n");
		_delay_ms(1000);
	}
}

