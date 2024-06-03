/*
 * EEPROM_TEST.c
 *
 * Created: 11/04/2023 15:33:50
 * Author : Fix-5
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
//#include <util/delay.h>
#include <avr/eeprom.h>


int main(void)
{
	DDRC = 0x0F;
	DDRD = 0x0F;
	
	uint8_t value = 15;
	
	MCUSR = 0;
	
	eeprom_write_byte((uint8_t*) 0x0000, value);
	
    while (1) 
    {
		PORTD = eeprom_read_byte((uint8_t*) 0x0000);
		PORTC = eeprom_read_byte((uint8_t*) 0x0000);
    }
}

