/*
 * LCD_I2C.c
 *
 * Created: 28/02/2023 10:36:15
 *  Author: Fix-5
 */ 

/* Contains I2C transactions */
#include "LCD_I2C.h"

/*
	DISPLAY_MODE permet de sélectionner 2 mode d'affichaege diférent.
	DISPLAY_MODE :
	0 --> 1 ligne
	1 --> 2 ligne
*/
static uint8_t DISPLAY_MODE = 1;

/*
	SCROLL_MODE permet de definir si oui ou non mon text defil
	SCROLL_MODE :
	0 --> pas de défilement
	1 --> avec défilement
*/
static uint8_t SCROLL_MODE = 0;


/*Global variable*/
static uint8_t buffer_SIZE = 0;
static uint8_t ErrorLCD;
static uint8_t actual_Val_Char = 0;/*valeur nombre de caractère actuel*/
static uint8_t actual_Val_Line = 0;/*valeur nombre lgine actuel*/
static char buffer_str[BUFFER_SIZE];
static uint8_t str_buffer_head = 0;
static uint8_t str_buffer_tail = 0;
static uint8_t strLength = 0;
static uint8_t data = 0;
static uint8_t valueCol = 0; 
static uint8_t valueLin = 0;


#ifdef _LCD_DEBUG_MODE
	static char uartMsg[100];
#endif


U_TWI_MSG i2cLcd;

/*Initialisation du LCD*/
/*
	initLCD plusieur choix de réglage et proposer :
	
		le réglage depend de la valer Hexadécimal que vous rentrer
		max --> 0x0F
		min --> 0x00
		
		noble de point fort pas utiliser
		nible de point faible : x x x x 2^3 2^2 2^1 2^0
		
		2^0 --> 1 => inc | 0 => dec  (curseur)
		
		2^1 --> 1 => blinking ON | 0 => blinking OFF  (curseur)
		
		2^2 --> 1 => visble | 0 => pas visible  (trait du curseur)
		
		2^3 --> 1 => 2 | 0 => 1 (choix du nombre de ligne en Hardware)
		
		Pour le 2^3, il n'est pas utile vus que je fait software.
		Donc de base a  2^3 == 1
			
	salveAdrr :
		l'adresse corespondant au slave
		
	code :
		gestion d'erreur, donc si le parametre initLCD dépasse 0x0F --> error
		
		rs bit a 0 pour envoyer les instructions
		rs bit a 1 pour envoyer les caractères
		
		ligne 93 à 104 des union pour séparer les valeur de initLCD
		et pouvoir apres initialiser différent paramètre
		
		délai de départ (datasheet)
		
		initialiser l'adresse avec la bibliotéque TWI_MASTER
		
		initialiser les data a 0 du lcd
		
		initialiser chaque data sur du 4 bits pour le lCD
		strobe de Enable pour chaque instruction (datasheet)
		
*/
uint8_t LCD_Init(uint8_t initLCD, uint8_t slaveAdrr)
{
	actual_Val_Char = 0;
	ctrlInit.byte = initLCD;
	
	if (ctrlInit.byte > 15)
	{
		ErrorLCD |= ERROR_INIT;
	}
	
	if (ErrorLCD == 0)
	{
		lcdCtrl.PcfRegs.rsBit = 0;
		
		/*valeur union action init*/
		/*pour le nombre de ligne*/
		ctrlLine.S_Set_Line.nbLine = ctrlInit.S_LCDInit.nb_line;
		
		/*pour le curseur*/
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


/*Send instrcution*/
/*
	permet d'envoyer un caractere ou une instruction
	suivant la valeur de type.
	
	instruction --> réglage du LCD
	caractère --> affichage sur l'écran
  
	type = 0 --> instruction
	type = 1 --> caractère
	
	code :
		"value" et séparer en 2 partie le nible fort et faible
		le fort et converti en faible pour l'envois en 4 bits avec l'union
		
		"type" et traiter avec une condition sois 0 sois 1 pour acitver
		le rs bit a 1 ou 0 pour les instruction et les caractéres.
		
		evois des 4 bits et strobe de E pour chaque envois (demande datasheet)
*/
void LCD_SendInstruction(char value, uint8_t type)
{
	uint8_t high_Value = 0;
	uint8_t low_Value = 0;
	
	/*isoler nible de point fort*/
	high_Value = (uint8_t)value >> 4;
	
	/*avoir que le nible de poids faible ex: 0x41 - 0x40 = 0x01*/
	low_Value = (uint8_t)value - (high_Value << 4);
	
	/*type pour pour instruction ou caractere*/
	if (type == 1)
	{
		lcdCtrl.PcfRegs.rsBit = 1;
	}
	else
	{
		lcdCtrl.PcfRegs.rsBit = 0;
	}
	
	
	/*traitement pour l'initialisation en 4 bits*/
	
	/*traitement nible fort*/
	lcdCtrl.PcfRegs.data = high_Value;
	i2cLcd.S_MsgSplit.Message[0] = lcdCtrl.pcfByte;
	TWIMaster_SendMsg(i2cLcd.MsgBuffer, 2);
	_delay_us(50);
	LCD_Strobe_E();
	
	/*traitement nible fiable*/
	lcdCtrl.PcfRegs.data = low_Value;
	i2cLcd.S_MsgSplit.Message[0] = lcdCtrl.pcfByte;
	TWIMaster_SendMsg(i2cLcd.MsgBuffer, 2);
	_delay_us(50);
	LCD_Strobe_E();
}


/*affiche un char*/
/*
	suivant le dislpay_MODE choisit donc 1 ou 2 ligne la taille du buffer 
	change
	pour une ligne --> 16
	pour deux ligne -> 32
	
	cela et utile au décalage ou au retour a la ligne
	
	il a une gestion d'erreur qui quand le Scroll_mode est à 0 donc sans 
	défilement et que la taille max de char de la ligne ou des 2 lignes 
	est dépasser --> error
	
	si pas d'error
		code :
			si l'affichage pas remplit, le caractére envoyer et mis en mémoire
			dans un buffer circulaire avec inc de l'index buffer head a chaque
			nouveau caractére
			
			ensuite nous récupere la valeur dans le buffer avec l'index buffer
			tail qui sera a se moment la, pareil que l'index buffer head et 
			nous affichons sur l'écrans a l'aide de la fonction 
			LCD_sendInstruction.
			
			incrémentation de la variable actual_val_Char pour savoir a 
			combien	de caractére nous somme
			
			si le DISPLAY_MODE est 1 donc 2 ligne et que actual_Val_Char = 16
			nous passons à la lgine suivante.
			
			si l'affichage remplie suivant le mode choisis bien sur et que le
			défilement et activer nous changeons de façon d'afficher
			
			ont affichera tout le buffer entier remplie a chaque nouveau 
			caractère ajouter.
			
			le nouveau caractère est ajouter dans le buffer avec 
			incrémentation de l'index buffer head pour le prochain caractère
			
			set le cursor en position 0,0
			
			on réaffiche le buffer en continue tant qu'un nouveau caractère et
			ajouter
				- si dans le for pour le buffer, i = 16 et que DISPLAY_MODE 
				  à 1 il y a passage a ligne suivante	
				- on récupere la donnée dans le buffer grace a l'index buffer 
				  tail et on l'affiche grace à LCD_sendInstruction(XxXX,1)
			
			après la boucle un delais de 500ms pour la fluiditer et la 
			lisibilité du text avec le décallage.	  	
*/
uint8_t LCD_PutChar(char C)
{		
	/*
		si le display est sur 1 ligne le buffer pour le defilement et de 16
		sinon 32 pour 2 ligne.
		
		16 et 32 --> nombre caractere sur l'affichage
	*/
	if (DISPLAY_MODE == 0)
	{
		buffer_SIZE = 16;
	} 
	else
	{
		buffer_SIZE = 32;
	}
	
	
	/*error max lengh char*/
	if (actual_Val_Char > buffer_SIZE && SCROLL_MODE == 0)
	{
		ErrorLCD |= ERROR_HIGH_LENGH_CHAR;
	}
	
	if (ErrorLCD == 0)
	{
		/*si l'affichage pas remplie*/
		if (actual_Val_Char < buffer_SIZE)
		{
			/*ligne suivante si la premiere est remplie*/
			if (actual_Val_Char == MAXCHAR_LINE_1_LINE && DISPLAY_MODE == 1)
			{
				LCD_setCursor(1,0);
				
				#ifdef _LCD_DEBUG_MODE
				Uart_PutString("Line 0 remplie *");
				_delay_ms(10);
				#endif
			}
			
			/*ajoute la nouvel valeur dans le circular buffer a la 
			position head_Index*/
			buffer_str[str_buffer_head] = C;
			str_buffer_head = (str_buffer_head + 1) % buffer_SIZE;
			
			/*isole char par char a la valeur tail_Index a envoyer*/
			data = buffer_str[str_buffer_tail];
			str_buffer_tail = (str_buffer_tail + 1) % buffer_SIZE;
			
			/*affiche le char isoler*/
			LCD_SendInstruction(data,1);

			actual_Val_Char++;

			#ifdef _LCD_DEBUG_MODE
			sprintf(uartMsg, "%2d*", actual_Val_Char);
			Uart_PutString(uartMsg);
			#endif
			
			#ifdef _LCD_TIME_SCROLL
			_delay_ms(250);
			#endif
		}
		else
		{
			/*si defilement definie ou pas*/
			if (SCROLL_MODE == 1)
			{
				/*ajouter la nouvel valeur*/
				buffer_str[str_buffer_head] = C;
				str_buffer_head = (str_buffer_head + 1) % buffer_SIZE;
				
				LCD_setCursor(0,0);
				
				/*fait tourner le buffer pour afficher en décaler le text*/
				str_buffer_tail = str_buffer_head;
				
				/*affichage du buffer circulaire*/
				for (int i = 0; i <= buffer_SIZE; i++)
				{
					/*ligne suivante si la premiere est remplie*/
					if (i == MAXCHAR_LINE_1_LINE && DISPLAY_MODE == 1)
					{
						LCD_setCursor(1,0);
					}
					
					
					data = buffer_str[str_buffer_tail];
					str_buffer_tail = (str_buffer_tail + 1) % buffer_SIZE;
					
					LCD_SendInstruction(data,1);
				}
				
				
				#ifdef _LCD_TIME_SCROLL
				_delay_ms(500);
				#endif
			}
		}
	}
	
	return ErrorLCD;
}

/*afficher une string*/
/*
	strLength récupere combien de caractére il y a dans le string envoyer
	
	gestion d'erreur pour voir si le defilement n'est pas activer
		- si strlength et plus grand que 16 et que le mode d'affichage et a 0
			--> error
		- si strlength et plus grand que 32 et que le mode d'affichage et a 1
			--> error 
	si pas d'erreur envois des caractere à la fonction LCD_PutChar
	
	l'erreur est pris en comppte que si le défilement n'est pas activer 
*/
uint8_t LCD_PutString(char *string)
{		
	/*savoir le nombre de char*/
	strLength = strlen((const char *) string);
	
	/*
		Error si en mode sans défilement la chaine de caractère
		dépasse le max possible sur 1 ou 2 ligne.
	*/
	if (SCROLL_MODE == 0)
	{
		if (strLength > MAXCHAR_LINE_1_LINE && DISPLAY_MODE == 0)
		{
			ErrorLCD |= ERROR_HIGH_LENGH_CHAR;
		}
		if (strLength > MAXCHAR_LINE_2_LINE && DISPLAY_MODE == 1)
		{
			ErrorLCD |= ERROR_HIGH_LENGH_CHAR;
		}
	}
	
	/*si pas d'erreur traite char by char*/
	if (ErrorLCD == 0)
	{
		for (int i = 0; i < strLength; i++)
		{	
			LCD_PutChar(string[i]);
		}
	}
	
	return ErrorLCD;	
}

/*permet de set le cursor en position X et y (ou ligne et colonne)*/
/*
	lin --> 0 => line 1 
	lin --> 1 => line 2 (nombre max de ligne)
	
	col > 0 et < 15 (valeur max de la taile du LCD 16*2)
	
	sur les valeur des variable si-dessus il y a une geston d'erreur
	
	code :
		si lin est > 1 --> error
		si col nombre negatif ou > 15 --> error
			
		si pas error :
			valueCol récupere a quelle colonne on est
			valueLin récupere a quelle ligne on est pour la condition suivant
			actual_val_line récupere pour savoir a quelle ligne on est
			
			si valueLin = 1 --> valueLin = 0x40 (valeur datasheet p.43) cette
			valeur est l'addresse du début de la 2éme ligne du LCD.
			sinon valuelin = 0x00 valeur de l'addresse du début de la ligne 1
			
			envois de l'instruction à l'aide de LCD_sendInstruction
			la valeur "+ 0x08" et la valeur de l'instruction de la datasheet
			a avoir pour que le déplacement de curseur fonctionne (p.43)
			
			on additione la colonne avec la line forme un nombre héxa complet
			que je traite avec la fonction LCD_sendInstruction
*/
uint8_t LCD_setCursor(uint8_t lin, uint8_t col)
{	
	/*
		si valeur de line trop grand --> error
		si valeur trop grand Max caractere --> error
	*/
	
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
		valueCol = col;
		valueLin = lin;
		actual_Val_Line = lin;
	
		/*pour afficher sur la ligne du bas l'adresse commence par 0x4X*/
		if (valueLin == 1)
		{
			/*0x04 pour utilisation de l'union et l'envois en 4bits*/
			valueLin = 0x40;
			actual_Val_Char = col;
		}
		else
		{
			valueLin = 0x00;
			actual_Val_Char = col;
		}
	
		#ifdef _LCD_DEBUG_MODE
				sprintf(uartMsg, "%2d", valueLin);
				Uart_PutString(uartMsg);
		#endif			
		
		/*adresse datasheet rs rw DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
					1		0  0  1	  1   0   0   x   x   x   x
					2		-  -  -   1   0   0   x   x   x   x 
		1 = l'adresse a voir dans la datasheet obligatoir
		2 = la partie de l'adresse corespondant a la ligne 2 de l'écran
		le DB7  a 1 et important pour set le cursor a l'endroit voulu 
		+ 0x08 (-> DB7) pour avoir l'aresse complet*/
		LCD_SendInstruction((0x80 + valueLin) + valueCol, 0);
	}
	
	return ErrorLCD;
}


/*fait une pulse "E" pour l'evois des instruction ou caractére*/
void LCD_Strobe_E(void)
{
	_delay_us(500);
	lcdCtrl.PcfRegs.enableBit = 1;
	i2cLcd.S_MsgSplit.Message[0] = lcdCtrl.pcfByte;
	TWIMaster_SendMsg(i2cLcd.MsgBuffer, 2);
	_delay_us(500);
	lcdCtrl.PcfRegs.enableBit = 0;
	i2cLcd.S_MsgSplit.Message[0] = lcdCtrl.pcfByte;
	TWIMaster_SendMsg(i2cLcd.MsgBuffer, 2);
	_delay_us(500);
}

/*clear le LCD et met a la position X=0 Y=0 le cursor*/
void LCD_Clear(void)
{
	LCD_SendInstruction(0x01,0);
	
	actual_Val_Char = 0;
}

/*clear la line choisis*/
/*
	pour clear une ligne je mets des espace depuis le début jsuqua'a la fin
	line --> 0 clear line 1
	line --> 1 clear line 2
	
	error :
		si la valeur mise dans l'argument et plus grand qu1 cela fait 
		une erreur
		
	code :
		clear la line avec des espace et reviend a la position 0,0 ou 1,0
		de la ligne clear
*/
uint8_t LCD_ClearLine(uint8_t line)
{
	actual_Val_Char = 0;
	
	/*clear line 1*/
	if (line == 0)
	{
		LCD_setCursor(0,0);
		for (int i = 0; i < 16; i++)
		{
			LCD_PutChar(' ');
		}
		LCD_setCursor(0,0);
	}
	
	/*clear line 2*/
	else if (line == 1)
	{
		LCD_setCursor(1,0);
		for (int i = 0; i < 16; i++)
		{
			LCD_PutChar(' ');
		}
		LCD_setCursor(1,0);
	}
	
	/*error --> si valeur de lin pas existant ou trop grand*/
	else
	{
		ErrorLCD |= ERROR_CLEAR_LINE;
	}
	
	return ErrorLCD;
}