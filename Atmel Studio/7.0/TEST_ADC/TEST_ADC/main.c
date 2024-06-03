/*
 * TEST_ADC.c
 *
 * Created: 06/02/2023 14:20:25
 * Author : Fix-5
 */ 


#define F_CPU 16000000UL
#include <avr/io.h>
#include "AVR_ADC.h"
#include "AVR_Uart.h"
#include <string.h>
#include <stdint-gcc.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int main(void)
{
	
	DDRD = 0x3F;
	DDRC = 0x0F;
	
    /* Replace with your application code */
	ADC_Init(1,128);
	
	UART_Init();
	
	sei();
	
    while (1) 
    {
		PORTD = ADC_ReadMeanValue(0x01, 255);
    }
}

