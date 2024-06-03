/*
 * TEST_Level_shifter_mosfet.c
 *
 * Created: 10.10.2023 08:29:16
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
    DDRC = 0x01;
	
    while (1) 
    {
		PORTC ^= 0x01;
		_delay_ms(1000);
    }
}

