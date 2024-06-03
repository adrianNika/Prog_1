/*
 * ex-mémoire.c
 *
 * Created: 14.09.2023 09:33:31
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
//#include <stdint-gcc.h>
//#include <util/delay.h>
//#include <avr/eeprom.h>


int main(void)
{
	PORTC = 0x00;
	PORTD = 0x0F;
    /* Replace with your application code */
    while (1) 
    {
		PORTD = 0x0F;
    }
}

