/*********************************************************************************
*	@file		AVR_LCD_1602_I2C.c
*	@brief		LCD Library 
*
*	General  Description of project
*	more info at bottom page
*
*
*
*	-File:		AVR_LCD_1602_I2C.c
*	-Compiler:	AVR-GCC
*	-MCU:		ATmega328

*	@Author		David Perrin
*	@version	1 
* 	@date		5/12/2023
*********************************************************************************/

#include "AVR_LCD_1602_I2C.h"

/* LCD INIT 

	When initializing, user has to decide several things
	    -----------------------------------------------------------------------
		- "EntryMode" :		I/D = inc (=1) or decrease cursor (=0)
							S = shift diplay (=1) OFF (= 0)
        Entry mode:
        |0|0|0|0|0|1|ID|S|
	    -----------------------------------------------------------------------
		- DisplayOnOff :	D = Display ON (=1) or OFF (=0)
							C = Cursor ON (=1) or OFF (=0)
							B = Cursor Blink (=1) or OFF (=0)
        DisplayOnOff:
        |0|0|0|0|1|D|C|B|
	    -----------------------------------------------------------------------				
		- CursoRDisplayShift (not needed):
							S/C = Display shift (=1) or cursor shift (=0)
							R/L = shift right (=1) or shift to the left (=0)
         CursoRDisplayShift:
         |0|0|0|1|S/C|R/L|-|-|                        
           
	    -----------------------------------------------------------------------							
		- FunctionSet:		DL = Datalen 8Bit (=1) or 4 bit (=0)
							N = 2 lines (=1) or 1 line (=0) 
							F = 5x10 dots (=1) or 5x8 dots (=0)
         FunctionSet:
         |0|0|1|D/L|N|F|-|-|
	    -----------------------------------------------------------------------

    Initialization is : 0x33, 0x32 (fixed values)
                        0x2C = FunctionSet -> 4bit, 2lines, 5x10dots
                        0x0C = Display onOff -> display ON, cursor & blonk off
                        0x06 = Entry Mode -> increase cursor & no display shift
                        0x01 = Clear the entire screen 
*/
uint8_t LCD_DEFAULT_INIT[LCD_INIT_LEN] = {0x33, 0x32, 0x2C, 0x0E, 0x06, 0x01};


/**
 * @brief  Initialize LCD
 *
 * LCD has to be initialized in order to work in 4 bit data mode. 
 * 
 * The initialization data are stored in "LCD_DEFAULT_INIT" table 
 * See the declaration of table for more information. 
 *
 * @param   lcd Structure pointer containing LCD information
 * @return void
 */
void LCD_Init(S_LCD_DATA *lcd)
{
    /* Initialization loop */
    for (uint8_t i = 0; i < LCD_INIT_LEN; i++)
    {
        LCD_SendInstruction(lcd, LCD_DEFAULT_INIT[i]);
        _delay_ms(1);
    }
    
    /*  In order to work with append mode, table has to be 
        filled with "BLANK" caracters  */
    for (uint8_t i = 0; i < LCD_CHAR_NUMBER; i++)
    {
        lcd->display[i] = ' ';
    }
}    


/**
 * @brief This function sends an instruction to the LCD (not a character)
          The difference is the data sent and the RS bit, which must be 
          cleared in order to send an Instruction (clear screen, go back home, etc) 
          
          See the datasheet & definitions in (.h) in order to see
          implemented instruction. 
          
 * @param  lcd Structure pointer containing LCD information
 * @param  Command (instruction) to be sent out
 * @return void 
 */
void LCD_SendInstruction(S_LCD_DATA *lcd, uint8_t command)
{
    /* Sending instruction --> RS = 0, RW = 0 */
    lcd->pcfdata.reg.rsBit = 0;
    lcd->pcfdata.reg.rwBit = 0;
    
    /* Sending Nibble "Hi" */
    lcd->pcfdata.reg.data = ((command >> 4) & 0x0F);
    lcd->i2c.S_MsgSplit.Message[0] = lcd->pcfdata.byte;
    TWIMaster_SendMsg(lcd->i2c.MsgBuffer, 2);
    _delay_ms(6);
    
    LCD_PulseEnable(lcd);
    
    /* Sending nibble "Lo" */
    lcd->pcfdata.reg.data = (command & 0x0F);
    lcd->i2c.S_MsgSplit.Message[0] = lcd->pcfdata.byte;
    TWIMaster_SendMsg(lcd->i2c.MsgBuffer, 2);
    _delay_ms(2);
    
    LCD_PulseEnable(lcd);
}

/**
 * @brief LCD_PulseEnable : This function create a enable strobe pulse 
                            to send data to the LCD
                                          ____________
                           enable pin ___|  150us ON  |___150 us OFF___
 * @param  lcd Structure pointer containing LCD information 
 * @return void
 */
void LCD_PulseEnable(S_LCD_DATA *lcd)
{
    lcd->pcfdata.reg.enableBit = 0x01;
    lcd->i2c.S_MsgSplit.Message[0] = lcd->pcfdata.byte;
    TWIMaster_SendMsg(lcd->i2c.MsgBuffer, 2);
    
    _delay_us(150);
    
    lcd->pcfdata.reg.enableBit = 0x00;
    lcd->i2c.S_MsgSplit.Message[0] = lcd->pcfdata.byte;
    TWIMaster_SendMsg(lcd->i2c.MsgBuffer, 2);
    
    _delay_us(150);

}


/**
 * @brief LCD_Putchar : This function sends a char in XCursor/YCursor position
                        If append mode enabled, this fuction shifts the display
                        in order to keep the last 32 char  ( horizontal scroll)
 * @param  lcd Structure pointer containing LCD information
 * @param  c char to be written in LCD
 * @return uint8_t
 */
uint8_t LCD_Putchar(S_LCD_DATA *lcd, char c)
{
    uint8_t error = 0; 
    
    /* Check if append mode is enabled. If so -> put char */
    if (lcd->appendMode != 0)
    {
        // if append mode is enbled, must check for column max value
        if ((lcd->cursorX) > (LCD_WIDTH - 1))
        {
            // "carriage return" 
            // @todo: change magic number for "line value"
            if (lcd->cursorY < (LCD_LINE_NB - 1))
            {
                // Go to next line
                lcd->cursorY ++;
                
                // If in first line, go to second line & first column              
                LCD_SetCursor(lcd, lcd->cursorY, 0);
                
                // Updates Table Index
                lcd->tableIndex = (lcd->cursorY * LCD_WIDTH) + lcd->cursorX; 
                lcd->display[lcd->tableIndex] = c;
                
                // Send to LCD
                LCD_SendChar(lcd, c);
                lcd->cursorX++;
            }
            else
            {
                // increase index 
                lcd->tableIndex ++;
                
                // Set to 0 if higher than max number of char
                lcd->tableIndex %= LCD_CHAR_NUMBER;
                
                // put char in buffer
                lcd->display[lcd->tableIndex] = c; 
                
                // LCD_refresh
                LCD_ShiftLeft(lcd);
            }
            
        }
        else
        {
            // max column not reached --> Sends char
            // Save char to memory
            lcd->tableIndex = (lcd->cursorY * LCD_WIDTH) + lcd->cursorX;
            lcd->display[lcd->tableIndex] = c;
            
            // Send to LCD
            LCD_SendChar(lcd, c);
            lcd->cursorX++;
        }
    }
    else
    {
        if ((lcd->cursorX) <= LCD_WIDTH)
        {
            // Save char to memory
            lcd->tableIndex = (lcd->cursorY * LCD_WIDTH) + lcd->cursorX;
            lcd->display[lcd->tableIndex] = c;
                    
            // Send to LCD
            LCD_SendChar(lcd, c);
            lcd->cursorX++;
        }
        else
        {
            error |= 1;
        }
    }
    return error;
}


/**
 * @brief Write a string to the LCD display.
 *
 * This function writes a null-terminated string to the LCD display
 * at the current cursor position. It also handles append mode and 
 * checks if the string fits within the display's width.
 *
 * @param lcd A pointer to an S_LCD_DATA structure representing the LCD state.
 * @param str A pointer to the null-terminated string to be displayed.
 *
 * @return uint8_t Returns a non-zero error code if the operation failed; otherwise 0.
 */
uint8_t LCD_PutString(S_LCD_DATA *lcd, char *str)
{
    uint8_t error = 0;
    
    uint8_t charNumber = strlen(str);
    
    /* Check if append mode enabled */
    if (lcd->appendMode != 0)
    {
        /* String append is activated, only accept to append 31
           char, no temporary buffer if more */
        
        if (charNumber < LCD_CHAR_NUMBER)
        {
            for (uint8_t i = 0; i < charNumber; i++)
            {
                error |= LCD_Putchar(lcd, str[i]);
            }
        }
        else
        {
            // To do : handle errors
        }
        
    }
    else
    {
        // check if string can be placed in buffer
        // Verify if data goes over width 
        if ((lcd->cursorX + charNumber) <= LCD_WIDTH)
        {
            // string can be shown in display,
            // Send char by char
            for (uint8_t i = 0; i < charNumber; i++)
            {
                error |= LCD_Putchar(lcd, str[i]);
            }
        }
       else
       {
            // error, width free char not sufficient
            error |= 1;
       }
    }
    
   return error;
}

/**
 * @brief Send a character to the LCD display.
 *
 * This function sends a character to the LCD display using a 4-bit interface.
 * The character is sent as two 4-bit nibbles (high nibble first).
 *
 * @param lcd A pointer to an S_LCD_DATA structure representing the LCD state.
 * @param c The character to be sent to the display.
 */
void LCD_SendChar(S_LCD_DATA *lcd, char c)
{
    /* Sending instruction --> RS = 0, RW = 0 */
    lcd->pcfdata.reg.rsBit = 1;
    lcd->pcfdata.reg.rwBit = 0;
        
    /* Sending Nibble "Hi" */
    lcd->pcfdata.reg.data = ((c >> 4) & 0x0F);
    lcd->i2c.S_MsgSplit.Message[0] = lcd->pcfdata.byte;
    TWIMaster_SendMsg(lcd->i2c.MsgBuffer, 2);
    _delay_us(300);
        
    LCD_PulseEnable(lcd);
        
    /* Sending nibble "Lo" */
    lcd->pcfdata.reg.data = (c & 0x0F);
    lcd->i2c.S_MsgSplit.Message[0] = lcd->pcfdata.byte;
    TWIMaster_SendMsg(lcd->i2c.MsgBuffer, 2);
    _delay_us(300);
        
    LCD_PulseEnable(lcd);
}


/**
 * @brief Set the cursor position on the LCD display.
 *
 * This function sets the position of the cursor on the LCD display.
 * It only accepts valid positions within the display's dimensions 
 * (2 lines of 16 columns).
 * @param lcd A pointer to an S_LCD_DATA structure representing the LCD state.
 * @param line The line number where to set the cursor (0-1).
 * @param column The column number where to set the cursor (0-15).
 *
 * @return uint8_t Returns a non-zero error code if the operation failed 
 */

uint8_t LCD_SetCursor(S_LCD_DATA * lcd, uint8_t line, uint8_t column)
{
    uint8_t error = 0; 
    uint8_t command = 0;
    
    if ((line <= (LCD_LINE_NB-1)) && (column <= (LCD_WIDTH-1)))
    {
        command = line;
        command *= 0x40;
        command += column;
        command |= 0x80;
        
        LCD_SendInstruction(lcd, command);
        lcd->cursorY = line;
        lcd->cursorX = column;
    }
    else
    {
        error = 1;
    }
    
    return error; 
}

/**
 * @brief Clear the LCD display.
 *
 * This function clears the LCD display and resets the cursor position 
 * and the table index. It also fills the display buffer with spaces.
 *
 * @param lcd A pointer to an S_LCD_DATA structure representing the LCD state.
 */
void LCD_Clear(S_LCD_DATA *lcd)
{
    LCD_SendInstruction(lcd,0x01);
    lcd->cursorX = 0x00;
    lcd->cursorY = 0x00;
    lcd->tableIndex = 0x00;
    
    /* Sets spaces (avoid error in append mode) */
    for (uint8_t i = 0; i < LCD_CHAR_NUMBER; i++)
    {
        lcd->display[i] = 0x10; 
    }
}

/**
 * @brief Shift the display content to the left.
 *
 * This function shifts the entire content of the LCD display 
 * one position to the left.
 * It maintains the circularity of the display content buffer 
 * and updates the cursor position accordingly.
 * 
 * @param lcd A pointer to an S_LCD_DATA structure representing the LCD state.
 */
void LCD_ShiftLeft(S_LCD_DATA *lcd)
{
    
    uint8_t tmpindex = lcd->tableIndex; 
    
    // Sets cursor at start
    LCD_SendInstruction(lcd,0x01);
    
    LCD_SetCursor(lcd, 0, 0);
    
    // Line 
    for (uint8_t i = 0; i < LCD_CHAR_NUMBER; i++)
    {
        if ((tmpindex + 1) < LCD_CHAR_NUMBER)
        {
            tmpindex ++; 
        }
        else
        {
            tmpindex = 0;
        }
        
        if (i == LCD_WIDTH)
        {
            LCD_SetCursor(lcd, 1, 0);
        }
        
        LCD_SendChar(lcd, lcd->display[tmpindex]);
    }
    
    // TO do : remove this shit code
    lcd->cursorX = LCD_WIDTH;
    lcd->cursorY = (LCD_LINE_NB-1);
}