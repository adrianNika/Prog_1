/*
 * SIMU_BAT_ADC.c
 *
 * Created: 06/07/2023 08:34:57
 * Author : Fix-5
 */ 


#include <avr/io.h>
#include <stdio.h>
#include "AVR_ADC.h"
#include "AVR_LCD_1602_I2C.h"
#include "TWI_Master.h"
#include "AVR_Uart.h"
#include "global.h"

uint16_t adcValue = 0;
uint16_t secAdcVal = 0;
float tension = 0;
char uartMsg[20];

int main(void)
{
	S_LCD_DATA lcd = {  .i2c.S_MsgSplit.SlaveAdress = 0x25,
						.i2c.S_MsgSplit.ReadWriteBit = TWI_WRITE,
						.cursorX = 0x00,
						.cursorY = 0x00,
						.tableIndex = 0x00,
						.appendMode = 0x00,
						.error = 0x00,
						.pcfdata.reg.backlight = 0x01};
	
	sei();
	TWIMaster_Init();
	LCD_Init(&lcd);
	ADC_Init(ADCREF_AVCC,128);
	UART_Init();
	
    /* Replace with your application code */
    while (1) 
    {
		adcValue = ADC_ReadMeanValue(0,255);
		
		tension = ((float)adcValue / 1024) * 5.081;
		
		if (adcValue != secAdcVal)
		{
			if (adcValue < secAdcVal - 2 || adcValue > secAdcVal + 2)
			{
				LCD_Clear(&lcd);
				LCD_SetCursor(&lcd,0,0);
				sprintf(uartMsg, "ADC val : %d;",adcValue);
				Uart_PutString(uartMsg);
				LCD_PutString(&lcd,uartMsg);
				LCD_SetCursor(&lcd,1,0);
				
				sprintf(uartMsg, "Tension : %1.3fV;",tension);
				Uart_PutString(uartMsg);
				Uart_PutString(";");
				LCD_PutString(&lcd,uartMsg);
				
				secAdcVal = adcValue;
			}
		}
    }
}