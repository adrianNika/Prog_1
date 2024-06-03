/*
 * LCD_I2C.c
 *
 * Created: 28/02/2023 10:36:15
 *  Author: Fix-5
 */ 

/* Contains I2C transactions */
#include "LCD_I2C.h"

/*Initialisation du LCD*/
/*choix du nombre de line 0=1 ligne 1=2 ligne*/
/*cursor ON/OFF 0=pas de cursor 1=avec cursor*/
/*Blliking ON/OFF 0=pas de bliking 1=avec bliking*/
/*cursor decrement ou incrment 0=dec 1=inc*/
/*l'adresse corespondant au slave*/
uint8_t LCD_Init(uint8_t initLCD, uint8_t slaveAdrr)
{
	ctrlInit.byte = initLCD;
	
	if (ctrlInit.byte > 15)
	{
		ErrorLCD |= ERROR_INIT;
	}
	
	if (ErrorLCD == 0)
	{
		lcdCtrl.PcfRegs.rsBit = 0;
		
		/*valeur union action init*/
		ctrlLine.S_Set_Line.nbLine = ctrlInit.S_LCDInit.nb_line;
		
		ctrlCursor.S_setCursor.blikingSet = ctrlInit.S_LCDInit.bliking_Set;
		ctrlCursor.S_setCursor.cursorSet = ctrlInit.S_LCDInit.cursor_set;
		ctrlCursor.S_setCursor.displayOn = 1;
		ctrlCursor.S_setCursor.isone = 1;
		
		ctrlDisplay.S_setDiplay.isZero1 = 0;
		ctrlDisplay.S_setDiplay.cursorIncDec = ctrlInit.S_LCDInit.incDec_cursor;
		ctrlDisplay.S_setDiplay.isone = 1;
		ctrlDisplay.S_setDiplay.isZero2 = 0;
		
		/* Start-up delay */
		_delay_ms(100);
		
		/* Initialize with adress */
		i2cLcd.S_MsgSplit.SlaveAdress = slaveAdrr;
		i2cLcd.S_MsgSplit.ReadWriteBit = TWI_WRITE;
		
		/* Initialize with LCD data*/
		lcdCtrl.PcfRegs.rsBit = 0;
		lcdCtrl.PcfRegs.rwBit = 0;
		lcdCtrl.PcfRegs.enableBit = 0;
		lcdCtrl.PcfRegs.backlight = 1;
		lcdCtrl.PcfRegs.data = 0;
		
		/*all data init*/
		uint8_t buffer_Data[] = {	0x03,
									0x03,
									0x03,
									0x02,
									0x02,
									ctrlLine.byte,
									0x00,
									0x08,
									0x00,
									0x01,
									0x00,
									ctrlDisplay.byte,
									0x00,
									ctrlCursor.byte};
		
		/*send all data init*/
		for (int i = 0; i < 14; i++)
		{
			lcdCtrl.PcfRegs.data = buffer_Data[i];
			i2cLcd.S_MsgSplit.Message[0] = lcdCtrl.pcfByte;
			TWIMaster_SendMsg(i2cLcd.MsgBuffer, 2);
			_delay_ms(10);
			LCD_Strobe_E();
		}
	}
	
	return ErrorLCD;
}

/*affiche un char*/
uint8_t LCD_PutChar(char C)
{
	
	uint8_t high_Value = 0;
	uint8_t low_Value = 0;
	
	/*valeur max de la ligne atteint*/
	if (actual_Val_Cursor == MAXCHAR)
	{
		ErrorLCD |= ERROR_HIGH_LENGH_CHAR;
	}
	
	if (ErrorLCD == 0)
	{
		actual_Val_Cursor++;
		/*avoir la data nible poids fort sur le nible de piods faible ex:0x41 -> 0x04*/
		high_Value = (int)C >> 4;
		
		lcdCtrl.PcfRegs.rsBit = 1;
		lcdCtrl.PcfRegs.data = high_Value;
		i2cLcd.S_MsgSplit.Message[0] = lcdCtrl.pcfByte;
		TWIMaster_SendMsg(i2cLcd.MsgBuffer, 2);
		_delay_ms(5);
		LCD_Strobe_E();
		
		/*avoir que le nible de poids faible ex: 0x41 - 0x40 = 0x01*/
		low_Value = (int)C - (high_Value << 4);
		
		lcdCtrl.PcfRegs.data = low_Value;
		i2cLcd.S_MsgSplit.Message[0] = lcdCtrl.pcfByte;
		TWIMaster_SendMsg(i2cLcd.MsgBuffer, 2);
		_delay_ms(5);
		LCD_Strobe_E();
	}				
	
	return ErrorLCD;				
}

/*afficher une string*/
uint8_t LCD_PutString(char *string)
{	
	/*savoir le nombre de char*/
	uint8_t strLength = strlen((const char *) string);
	
	/*envois char par char*/
	for (int i = 0; i < strLength; i++)
	{
		LCD_PutChar(string[i]);
	}
	return ErrorLCD;
}

/*permet de set le cursor en position X et y (ou ligne et colonne)*/
uint8_t LCD_setCursor(uint8_t lin, uint8_t col)
{	
	if (lin > 1)
	{
		ErrorLCD |= ERROR_CURSOR;
	}
	if (col < 0 || lin > 15)
	{
		ErrorLCD |= ERROR_CURSOR;
	}
	
	if (ErrorLCD == 0)
	{
		lcdCtrl.PcfRegs.rsBit = 0;
		uint8_t valueCol = col;
		actual_Val_Cursor = col;
		uint8_t valueLin = lin;
	
		/*pour afficher sur la ligne du bas l'adresse commence par 0x4X*/
		if (valueLin == 1)
		{
			/*0x04 pour utilisation de l'union*/
			valueLin = 0x04;
		}	
	
		/*adresse datasheet rs rw DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
					1		0  0  1	  1   0   0   x   x   x   x
					2		-  -  -   1   0   0   x   x   x   x 
		1 = l'adresse a voir dans la datasheet
		2 = la partie de l'adresse corespondant a la ligne 2 de l'écran
		le DB7  a 1 et important pour set le cursor a l'endroit voulu */
		/*+ 0x08 (-> DB7) pour avoir l'aresse complet*/
		lcdCtrl.PcfRegs.data = valueLin + 0x08;
		i2cLcd.S_MsgSplit.Message[0] = lcdCtrl.pcfByte;
		TWIMaster_SendMsg(i2cLcd.MsgBuffer, 2);
		_delay_ms(5);
		LCD_Strobe_E();
	
		lcdCtrl.PcfRegs.data = valueCol;
		i2cLcd.S_MsgSplit.Message[0] = lcdCtrl.pcfByte;
		TWIMaster_SendMsg(i2cLcd.MsgBuffer, 2);
		_delay_ms(5);
		LCD_Strobe_E();
	}
	
	return ErrorLCD;
}

/*fait une pulse E pour l'evois des données enregistrer*/
void LCD_Strobe_E(void)
{
	_delay_ms(5);
	lcdCtrl.PcfRegs.enableBit = 1;
	i2cLcd.S_MsgSplit.Message[0] = lcdCtrl.pcfByte;
	TWIMaster_SendMsg(i2cLcd.MsgBuffer, 2);
	_delay_ms(5);
	lcdCtrl.PcfRegs.enableBit = 0;
	i2cLcd.S_MsgSplit.Message[0] = lcdCtrl.pcfByte;
	TWIMaster_SendMsg(i2cLcd.MsgBuffer, 2);
	_delay_ms(5);
}

/*clear le LCD et met a la position X=0 Y=0 le cursor*/
void LCD_Clear(void)
{
	lcdCtrl.PcfRegs.rsBit = 0;
	
	lcdCtrl.PcfRegs.data = 0x00;
	i2cLcd.S_MsgSplit.Message[0] = lcdCtrl.pcfByte;
	TWIMaster_SendMsg(i2cLcd.MsgBuffer, 2);
	_delay_ms(5);

	LCD_Strobe_E();
	
	lcdCtrl.PcfRegs.data = 0x01;
	i2cLcd.S_MsgSplit.Message[0] = lcdCtrl.pcfByte;
	TWIMaster_SendMsg(i2cLcd.MsgBuffer, 2);
	_delay_ms(5);

	LCD_Strobe_E();
	
	actual_Val_Cursor = 0;
}

/*clear la line choisis*/
uint8_t LCD_ClearLine(uint8_t line)
{
	actual_Val_Cursor = 0;
	if (line == 0)
	{
		LCD_setCursor(0,0);
		for (int i = 0; i < 16; i++)
		{
			LCD_PutChar(' ');
		}
		LCD_setCursor(0,0);
	}
	else if (line == 1)
	{
		LCD_setCursor(1,0);
		for (int i = 0; i < 16; i++)
		{
			LCD_PutChar(' ');
		}
		LCD_setCursor(1,0);
	}
	else
	{
		ErrorLCD |= ERROR_CLEAR_LINE;
	}
	
	return ErrorLCD;
}