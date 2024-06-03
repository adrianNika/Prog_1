/*
 * TEST-biblio-LCD.c
 *
 * Created: 01/03/2023 11:38:01
 * Author : Fix-5
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "LCD_I2C.h"
#include "TWI_Master.h"


int main(void)
{
	uint8_t test = 0;
	TWIMaster_Init();
	sei();
	
	test = LCD_Init(0x0F,0x25);
	
	LCD_setCursor(0 , 5);
	
	char str[] = "Bonjour";
	
    /* Replace with your application code */
    while (1) 
    {
		LCD_PutString(str);
		
		LCD_setCursor(1,2);
		LCD_PutString("et Bienvenue");
		
		
		while(1);
    }
}

