/*
 * LCD-letter-H.c
 *
 * Created: 22/02/2023 14:09:43
 * Author : Fix-5
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "AVR_Uart.h"
#include "TWI_Master.h"
#include "LCD_I2C.h"


int main(void)
{
	DDRD = 0x00;
	
	UART_Init();
	TWIMaster_Init();
	sei();
	
	/*Nombre de line,
	  set du cursor on/off,
	  set du bliking on/off,
	  inc dec cursor,
	  set display on/off */
	LCD_Init(0x0F, 0x25);
	LCD_setCursor(0,0);

	char str[] = "Bonjour et bienvenu sur un LCD 16*2 code en 4 bits";
	
	while (1)
	{
		/*evois un string*/
		LCD_PutString(str);
		

#ifdef _LCD_DEBUG_MODE			
		Uart_PutString("----------------------------------*");
#endif		
		while(1);

	}
}
