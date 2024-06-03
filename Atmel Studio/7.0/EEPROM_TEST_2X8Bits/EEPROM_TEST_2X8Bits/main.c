/*
 * EEPROM_TEST_2X8Bits.c
 *
 * Created: 06/06/2023 08:12:23
 * Author : Fix-5
 */ 

#define	F_CPU	8000000UL
#include <avr/io.h>
#include "AVR_Uart.h"
#include <avr/eeprom.h>

uint16_t score = 666;
uint8_t high_value_s = 0;
uint8_t low_value_s = 0;
uint16_t bestscore = 0;
char uartMsg[] = "";


int main(void)
{
    /* Replace with your application code */
	
	UART_Init();
	sei();

    while (1) 
    {
		high_value_s = (score >> 8) & 0xFF;
		low_value_s = score & 0xFF;
		
		eeprom_write_byte((uint8_t *)0, (score >> 8) & 0xFF);
		eeprom_write_byte((uint8_t *)1, score & 0xFF);
		
		bestscore = (eeprom_read_byte((uint8_t *)0) << 8);
		bestscore = bestscore + eeprom_read_byte((uint8_t *)1);
		
		sprintf(uartMsg, "%d*", bestscore);
		
		Uart_PutString(uartMsg);
		
		while(1);
    }
}

