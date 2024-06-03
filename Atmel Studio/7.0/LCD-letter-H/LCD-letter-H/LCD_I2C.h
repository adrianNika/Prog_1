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

/*define debug_Mode*/
//#define _LCD_DEBUG_MODE 1
/*define debug_Mode*/

#ifdef _LCD_DEBUG_MODE
	#include <stdio.h>
#endif

/*define taill buffer et defintion de la taille max des lignes*/
#define MAXCHAR_LINE_1_LINE 16
#define MAXCHAR_LINE_2_LINE 32
#define BUFFER_SIZE 32

/*error flag*/
#define ERROR_INIT 1
#define ERROR_CURSOR 2
#define ERROR_HIGH_LENGH_CHAR 4
#define ERROR_CLEAR_LINE 8


/*union et struct pour controler du rs rw enable et backlight*/
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
/*reglage LCD datasheet p.24-25*/
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

/*masquage pour contrle du cursor, bliking, display on/off*/
/*réglage LCD datasheet p.24-25*/
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
/*réglage LCD datasheet p.24-25*/
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

/*controle cursor */
/*réglage LCD datasheet p.24-25*/
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
	
	/*send instruction pour caractère ou instruction*/
	void LCD_SendInstruction(char value, uint8_t type);
	
	/*strobe du enable pour envoyer les data*/				
	void LCD_Strobe_E(void);
	
	/*
		afficher un char (mode affichage et défilement sélectionnable avec 
		variable global en haut du .C)
	*/
	uint8_t LCD_PutChar(char C);
				
	/*afficher un string*/							
	uint8_t LCD_PutString(char *string);							

	/*set le cursor*/
	uint8_t LCD_setCursor(uint8_t lin, uint8_t col);
	
	/*clear LCD*/				
	void LCD_Clear(void);
	
	/*clear Line LCD*/											
	uint8_t LCD_ClearLine(uint8_t line);							

#endif /* LCD_I2C_H_ */