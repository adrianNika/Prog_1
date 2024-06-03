/*********************************************************************************
*	@file		main.c
*	@brief		TEST_LCD
*
*	Description sommaire du projet 
*
*
*	-File:		main.c
*	-Compiler:	AVR-GCC
*	-MCU:		ATMEGA328
*
*	@Author		D. Perrin
*	@version	1
* 	@date		21.06.2023
*********************************************************************************/

#include "global.h"
#include "TWI_Master.h"
#include "AVR_LCD_1602_I2C.h"
#include <stdio.h>


int main(void)
{
    /* Start-up delay */
    _delay_ms(100);
    
    /* Structure for LCD Data */
    S_LCD_DATA lcd = {  .i2c.S_MsgSplit.SlaveAdress = 0x27,
                        .i2c.S_MsgSplit.ReadWriteBit = TWI_WRITE,       
                        .cursorX = 0x00,
                        .cursorY = 0x00,
                        .tableIndex = 0x00,
                        .appendMode = 0x01,
                        .error = 0x00,      
                        .pcfdata.reg.backlight = 0x01};

    sei();

    /* Initialize I2C + LCD */
    TWIMaster_Init();
	LCD_Init(&lcd);
	
	/* Testing LCD screen */
	LCD_PutString(&lcd,"QUOI ? ");
	LCD_SetCursor(&lcd, 1, 0);
	LCD_PutString(&lcd, "QUOICOUBEEEEHHH");
    
    while (1)
    {
		/* MAIN CODE APPLICATION HERE*/
    }
}
