/*
 * LCD_I2C.h
 *
 * Created: 28/02/2023 10:36:29
 *  Author: Fix-5
 */ 
#ifndef LCD_I2C_H_
#define LCD_I2C_H_

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "TWI_Master.h"
#include <string.h>

// Using standard types
#include <stdint-gcc.h>

#define MAXCHAR 16

/*error flag*/
#define ERROR_INIT 1
#define ERROR_CURSOR 2
#define ERROR_HIGH_LENGH_CHAR 4
#define ERROR_CLEAR_LINE 8

static volatile uint8_t ErrorLCD;
static volatile uint8_t actual_Val_Cursor = 0;

U_TWI_MSG i2cLcd;

typedef	union{
	struct
	{
		uint8_t rsBit:1;
		uint8_t rwBit:1;
		uint8_t enableBit:1;
		uint8_t backlight:1;
		uint8_t data:4;
	}PcfRegs;
	
	uint8_t pcfByte;
}U_pcfData;

U_pcfData lcdCtrl;

/*masquage pour choix du nombre de line*/
typedef union
{
	struct
	{
		uint8_t noUsed1:3;
		uint8_t nbLine:1;
		uint8_t noUsed2:4;
	}S_Set_Line;
	
	uint8_t byte;
}U_Line;

U_Line ctrlLine;

/*masquage pour contrle du cursor,bliking etc. on/off*/
typedef union
{
	struct
	{
		uint8_t blikingSet:1;
		uint8_t cursorSet:1;
		uint8_t displayOn:1;
		uint8_t isone:1;
		uint8_t noUsed1:4;
	}S_setCursor;
	
	uint8_t byte;
}U_setCursor;

U_setCursor ctrlCursor;

/*controle inc dec cursor*/
typedef union
{
	struct
	{
		uint8_t isZero1:1;
		uint8_t cursorIncDec:1;
		uint8_t isone:1;
		uint8_t isZero2:1;
		uint8_t noUsed1:4;
	}S_setDiplay;
	
	uint8_t byte;
}U_setDisplay;

U_setDisplay ctrlDisplay;

typedef union
{
	struct
	{
		uint8_t incDec_cursor:1;
		uint8_t bliking_Set:1;
		uint8_t cursor_set:1;
		uint8_t nb_line:1;
		uint8_t noUsed:4;
	}S_LCDInit;
	
	uint8_t byte;
}U_LCDInit;

U_LCDInit ctrlInit;

/*initialisation du LCD*/
uint8_t LCD_Init(uint8_t initLCD, uint8_t slaveAdrr);
					
void LCD_Strobe_E(void);						/*strobe du enable pour envoyer les data*/
uint8_t LCD_PutChar(char C);						/*afficher un char*/
uint8_t LCD_PutString(char *string);				/*afficher un string*/
uint8_t LCD_setCursor(uint8_t lin, uint8_t col);	/*set le cursor*/
void LCD_Clear(void);							/*clear LCD*/
uint8_t LCD_ClearLine(uint8_t line);

#endif /* LCD_I2C_H_ */