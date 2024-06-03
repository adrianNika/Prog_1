/*
 * ex_memoire.c
 *
 * Created: 14.09.2023 09:39:04
 * Author : adria
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
//#include "AVR_Uart.h"
//#include "TWI_Master.h"
//#include "LCD_I2C.h"
#include <stdio.h>
//#include <avr/sleep.h>
//#include <avr/interrupt.h>
#include <stdint-gcc.h>
#include <util/delay.h>
//#include <avr/eeprom.h>



int main(void)
{
    PORTD = 0x0F;
	PORTC = 0x00;
	
	int valeurLue = 0;
	
    while (1) 
    {
		if (bit_is_set(PINC,0))
		{
			valeurLue = PORTC;
		}
		PORTD = valeurLue;
    }
}

