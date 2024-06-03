/******************************************************************************
*	@file		AVR_LCD_1602_I2C.h
*	@brief		Include File for LCD library (1602, I2C) 
*
*	General  Description of Library
*
*
*	-File:		AVR_LCD_1602_I2C.h
*	-Compiler:	AVR-GC
*	-MCU:		ATmega328

*	@Author		David Perrin
*	@version	1 
* 	@date		5/12/2023
******************************************************************************/

#ifndef AVR_LCD_1602_I2C_H_
#define AVR_LCD_1602_I2C_H_

/**************** USER INCLUDES **********************************************/

#include "global.h"
#include "TWI_Master.h"
#include <string.h>


/**************** USER DEFINES ***********************************************/


#define LCD_INIT_LEN 6

#define LCD_CHAR_NUMBER 32

#define LCD_WIDTH 16
#define LCD_LINE_NB 2

/**************** USER TYPES *************************************************/

/* This union simplyfies the bit handling data
   With PCF8574 connected to the LCD driver */
typedef	union{
	struct  
	{
		uint8_t rsBit:1;
		uint8_t rwBit:1;
		uint8_t enableBit:1;
		uint8_t backlight:1;
		uint8_t data:4;
	}reg;
	
	uint8_t byte;
}U_PCF_LCD;

/*
    This structure contains all data to handle a single LCD
    
    This permits to create a table of lcd, each ones containing
    their data. 
    
*/
typedef struct  
{
    U_TWI_MSG i2c;          /* I2C message structure (using TWI_MASTER) */
    U_PCF_LCD pcfdata;      /* How data is structured ( see PCF shematics) */  
    
    uint8_t cursorY;        /* Current "Y" position of cursor, start at 0. */
    uint8_t cursorX;        /* Current "X" position of cursor, start at 0. */
    
    uint8_t tableIndex;     /* Current index of screen (to do : modify name) */
    
    char display[LCD_CHAR_NUMBER];  /* What is shown on LCD  */
                                    
    
    uint8_t appendMode;     /* If append is enable, display data shifts  */
                            /* to do : create an union containing all specific 
                            data relating LCD */
    
    uint8_t error;          /* Contains error code */
      
}S_LCD_DATA;



/**************** USER PRIMITIVES ********************************************/
/*
    @todo Error handling correctly
*/
void LCD_Init(S_LCD_DATA *lcd);
void LCD_SendInstruction(S_LCD_DATA *lcd, uint8_t command);
void LCD_PulseEnable(S_LCD_DATA *lcd);
uint8_t LCD_Putchar(S_LCD_DATA *lcd, char c);
uint8_t LCD_PutString(S_LCD_DATA *lcd, char *str);
void LCD_SendChar(S_LCD_DATA *lcd, char c);
uint8_t LCD_SetCursor(S_LCD_DATA * lcd, uint8_t line, uint8_t column);
void LCD_Clear(S_LCD_DATA *lcd);
void LCD_ShiftLeft(S_LCD_DATA *lcd);



#endif /* AVR_LCD_1602_I2C_H_ */