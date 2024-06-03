/*
 * TEST_duellator.c
 *
 * Created: 11/05/2023 16:35:40
 * Author : Fix-5
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>

#include "LCD_I2C.h"
#include "global.h"
#include "AVR_Uart.h"

U_TWI_MSG led;

int main(void)
{	
	DDRB = 0x0F;
	DDRC = 0x0F;
	
	uint8_t flag = 0;
	uint8_t flag2 = 0;
	uint8_t k2000 = 1;
	uint8_t etatTest = 1;
	uint8_t etatLCD = 0;
	uint8_t i = 0;
	
	uint8_t table[] = {255,254,253,251,247,239,223,191,127};
	
	sei();
	
	/*multi Init*/
	UART_Init();
	TWIMaster_Init();
	LCD_Init(0x0F,0x25);
	
	/*i2c*/
	led.S_MsgSplit.SlaveAdress = 0x38;
	led.S_MsgSplit.ReadWriteBit = TWI_WRITE;

	led.S_MsgSplit.Message[0] = 255;
	TWIMaster_SendMsg(led.MsgBuffer , 2);
	while(TWI_isBusy());
	
	/*LCD*/
	LCD_Clear();
	
	LCD_setCursor(0,0);
	LCD_PutString("Welcome BOA");
	LCD_setCursor(1,0);
	LCD_PutString("From Atmega88PA");
	
	Uart_PutString("salut BOA*");
	_delay_ms(3000);
	
	LCD_ClearLine(1);
	
    while (1) 
    {
		PORTC ^= 0x0F;
		
		switch (etatTest)
		{
			case 0:
				if (flag2 == 0)
				{
					etatLCD = 0;
				}
				
				if (i < 9)
				{
					led.S_MsgSplit.Message[0] = table[i];
					TWIMaster_SendMsg(led.MsgBuffer , 2);
					while(TWI_isBusy());
					_delay_ms(500);
					i++;
				}
				else
				{
					i = 0;
				}
			break;
			
			case 1:
				if (flag2 == 0)
				{
					etatLCD = 1;
				}
				
				led.S_MsgSplit.Message[0] = ~k2000;
				TWIMaster_SendMsg(led.MsgBuffer , 2);
				while(TWI_isBusy());

				if (k2000 != 128 && flag == 0)
				{
					k2000 *= 2;
				}
				else
				{
					flag = 1;
					
					if (k2000 != 1 && flag == 1)
					{
						k2000 /= 2;
					}
					else
					{
						flag = 0;
					}
				}
				
				_delay_ms(35);
			break;
			
			case 2:
				if (flag2 == 0)
				{
					etatLCD = 2;
				}
			
				led.S_MsgSplit.Message[0] = 170;
				TWIMaster_SendMsg(led.MsgBuffer , 2);
				while(TWI_isBusy());
				_delay_ms(200);
				led.S_MsgSplit.Message[0] = 85;
				TWIMaster_SendMsg(led.MsgBuffer , 2);
				while(TWI_isBusy());
				_delay_ms(200);
			break;
			
			default:
			break;
		}
		
		switch (etatLCD)
		{
			case 0:
				LCD_ClearLine(1);
				LCD_setCursor(1,0);
				LCD_PutString("TEST inc.");
				Uart_PutString("TEST inc.");
				flag2 = 1;
				etatLCD = 3;
			break;		
			
			case 1:
				LCD_ClearLine(1);
				LCD_setCursor(1,0);
				LCD_PutString("TEST K2000");
				Uart_PutString("TEST K2000");
				flag2 = 1;
				etatLCD = 3;
			break;
			
			case 2:
				LCD_ClearLine(1);
				LCD_setCursor(1,0);
				LCD_PutString("TEST bliking");
				Uart_PutString("TEST bliking");
				flag2 = 1;
				etatLCD = 3;
			break;
			
			default:
				
			break;	
		}
    }
}

