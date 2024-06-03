/*
 * TEST-2-Duellator.c
 *
 * Created: 11/05/2023 16:42:34
 * Author : Fix-5
 */ 


#define F_CPU 8000000UL
#include <avr/io.h>
#include "LCD_I2C.h"
#include "global.h"


int main(void)
{
	LCD_Init(0x0F,0x25);
	/* Replace with your application code */
	
	LCD_setCursor(0,0);
	LCD_PutString("Welcome BOA");
	LCD_setCursor(1,0);
	LCD_PutString("From Atmega88PA");
	
	while (1)
	{
		
	}
}