/*********************************************************************************
*	@file		main.c
*	@brief		This program uses TIMER peripheral to decode morse code 
*
*	Project	"MORSE DECODER"	
*
*
*	-File:		main.c
*	-Compiler:	AVR-GCC
*	-MCU:		ATMEGA328

*	@Author		David Perrin
*	@version	0.1 
* 	@date		1/30/2023 - 8:32:42 AM
*********************************************************************************/

/* General dependencies of project */
#include "global.h"

/* Using UART library (ELO) */
#include "AVR_Uart.h"

#include <avr/sleep.h>


/* User Defines 
	Top value for a dot @15WPM = 1200/15 = 80 [ms] = 0,08 [s]
		dot duration = ((16000000 * 0.08)/1024) -1 = 1249 
		Dash duration = 3* dot duration = ((16000000 * 0.24)/1024) -1 = 3749  
		Space duration = 7* dot duration = ((16000000 * 0.56)/1024)-1 = 8749
  */

const uint16_t DOT_DURATION = 1249;

/* +- 36% for min & max values */
const uint16_t DOT_DURATION_MIN = 800;
const uint16_t DOT_DURATION_MAX = 1700;

const uint16_t DASH_DURATION = 3749;
/* +- 36% for min & max values */
const uint16_t DASH_DURATION_MIN = 2400;
const uint16_t DASH_DURATION_MAX = 5100;

const uint16_t SPACE_DURATION = 8749;

/* Those constants defines time to wait before 
   determination of the end of a char (1.5 * DOT) or 
   a space ( 1.5 + 2.5 DOT) 
*/
const uint16_t WAIT_END_CHAR = 1874;
const uint16_t WAIT_SPACE = 3124;



/* State machine for morse decoder */
typedef enum{
	STATE_WAIT_START_SIGN,
	STATE_WAIT_END_SIGN
}E_STATE_DECODER;

/* State machine variable */
E_STATE_DECODER	stateMorseDecoder = STATE_WAIT_START_SIGN;

/* MORSE TABLE */
/* Starting from "0" to "Z", if morse char doesn't exist -> replaces by NULL*/
const char *MORSE_TABLE[] = {"-----",	
							".----",	
							"..---",	
							"...--",	
							"....-",	
							".....",	
							"-....",	
							"--...",	
							"---..",	
							"----.",	
							"---...",	
							"-.-.-.",	
							"",			
							"-...-",	
							"",			
							"..--..",	
							".--.-.",	
							".-",		
							"-...",		
							"-.-.",		
							"-..",		
							".",		
							"..-.",		
							"--.",		
							"....",		
							"..",		
							".---",		
							"-.-",		
							".-..",		
							"--",		
							"-.",		
							"---",		
							".--.",		
							"--.-",		
							".-.",		
							"...",		
							"-",		
							"..-",		
							"...-",		
							".--",		
							"-..-",		
							"-.--",		
							"--.."};	

/* Global variables */
/* @ to do: Flags can be optimized with masks */
uint8_t flagEndOfMorseChar = 0;
uint8_t flagSendSpaceChar = 0;
uint8_t flagCharReading = 0; 
uint8_t flagCheckEndChar = 0;
uint8_t flagCheckSpace = 0; 
uint8_t captureIndex = 0;
int uartMsg1[100];

uint16_t tempCaptureValue = 0;

/* Table containing timings of signs  */
uint16_t captureTable[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/* Global variable containing TOP value (memory) */
uint16_t actualTop = 0xFFFF;



ISR(TIMER1_COMPA_vect)
{
	/* 3 possibilities for the program to end up here :
		a - 1.5 dot duration passed : end of char detected
		b - 4	dot duration passed : space detected
		c - overflow without flag : Only a rising edge detected */ 
	
	/* checks if falling edge detected */ 
	PORTD |= 0x40;
	cli();
	
	if(flagCheckEndChar != 0)
	{
		/* clears flag */
		flagCheckEndChar = 0;
		flagCharReading = 0;
		captureIndex = 0;
		
		/* Sets for another rising edge detection : 2.5 dot  */
		actualTop = WAIT_SPACE;
		OCR1A = actualTop;
		TIFR1 |= ((1 << TOV1) | (1 << OCF1A));
		flagCheckSpace = 0x01;
		
		/* flag decoder action  */
		flagEndOfMorseChar |= 0x01;		
	}
	else
	{
		/* check if space checking  */
		if (flagCheckSpace != 0)
		{
			/* 4 dot duration passed --> Stop Timer and rise 
				flag for sending a space char */
			flagCheckSpace = 0; 
			flagSendSpaceChar =1; 
			TCCR1B &= ~(0x05);
			TCNT1 = 0x0000;
			OCR1A = 0xFFFF;
			TIFR1 |= ((1 << TOV1) | (1 << OCF1A));
		}
	}
	
	sei();
	PORTD &= ~0x40;
}

ISR(TIMER1_CAPT_vect)
{
	PCICR &= ~(1 << PCIE0);
	
	PORTD |= 0x80;
	/* Deactivate  re-entering in the interrupt */
	cli();
	
	/* Save Capture value */
	tempCaptureValue = ICR1;
	
	/*  */
	switch (stateMorseDecoder)
	{
		case STATE_WAIT_START_SIGN: 
			
			if (flagCheckEndChar == 0x00)
			{
				/* Start capturing a new char --> reset timer */
				if (flagCharReading == 0)
				{
					flagCharReading |= 0x01;
					actualTop = 0xFFFF;
					OCR1A = actualTop;
					TCNT1 = 0x0000;
					tempCaptureValue = 0;
				
					/* Clears evt. flags */
					TCCR1B |= 0x05;
					TIFR1 |= ((1 << TOV1) | (1 << OCF1A));
				}
				else
				{
					/* should never be here */
				}
			}
			else
			{
				/* not a first char detection. If PC is here, 
				we continue to measure signs, either a dot or 
				a dash --> OCR value MUST be MAX */
				
				actualTop = 0xFFFF;
				OCR1A = actualTop; 
				/* Clears evt. flags */
				TIFR1 |= ((1 << TOV1) | (1 << OCF1A));
				
				flagCheckEndChar = 0;
			}
					
			stateMorseDecoder = STATE_WAIT_END_SIGN;		
		break;
		
		case STATE_WAIT_END_SIGN:
			/* If arrived here, sign duration in computable.
			The trick is to set a duration  */
			
			if (((uint32_t) tempCaptureValue + WAIT_END_CHAR) > 0xFFFF)
			{
				/* error, char is too long */
				
			}
			else
			{
				actualTop = tempCaptureValue + WAIT_END_CHAR;
				OCR1A = actualTop;
				flagCheckEndChar = 0x01;
			}
			
			stateMorseDecoder = STATE_WAIT_START_SIGN;
			
		break;
		
		default:
		break;
	}
	
	/* Save in table */
	captureTable[captureIndex] = tempCaptureValue;
	captureIndex ++;
	
	/* Change edge for next measurement ! */
	TCCR1B ^= (1 << ICES1);
		
	/* Debouncing, not optimal, but okay */
	_delay_ms(1);
	TIFR1 |= (1 << ICF1);
	sei();
	PORTD &= ~0x80;
}



int main(void)
{
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	
	PCMSK0 |= (1 << PCINT0);
	PCICR |= (1 << PCIE0);
	
	// Dummy Value
	char TxBuffer[UART_TX_BUFFER_SIZE];
	char MorseCharToDecode[8];
	uint8_t flagCharFound;
	uint8_t i = 0;
	uint8_t y = 0;
	uint16_t signDuration = 0x0000;
	uint32_t timeSleep = 0;

    // Initialization of UART
    UART_Init();
    	
	/*TIMER 1 CONFIG */
	TCCR1A = 0x00;
	
	/* TIMER 1 - CONFIG
		Inputcapture enables with filter (6..7) = 11
		bit 5 reserved							= 0
		MODE CTC (4..3)							= 01
		Prescaler 1024(2..0)					= 000
	*/
	TCCR1B = 0x00;
	TCCR1B |= 0b11001000;
	
	/* The two interrupts activated */
	OCR1A = 0xFFFF;
	TIMSK1 = 0x00;
	TIMSK1 |= ((1 << OCIE1A) | (1 << ICIE1));
	
	DDRD |=0xF0;
	
    // Permits interruptions
    sei();
	
	TWIMaster_Init();
	LCD_Init(0x0F,0x25);
	
	LCD_setCursor(0,0);
	
    while (1) 
    {
		timeSleep++;
		
		if (flagEndOfMorseChar != 0)
		{			
			timeSleep = 0;	
			
			flagEndOfMorseChar = 0; 
			
			/*Decoding morse code here*/
			y=0;
			
			for(i = 1; captureTable[i] != 0; i+=2 )
			{
				signDuration = (captureTable[i] - captureTable[(i-1)]);
				
				if ((signDuration > DOT_DURATION_MIN) && ((signDuration < DOT_DURATION_MAX)))
				{
					MorseCharToDecode[y] = '.';
				}
				else
				{
					if ((signDuration > DASH_DURATION_MIN) && ((signDuration < DASH_DURATION_MAX)))
					{
						MorseCharToDecode[y] = '-';
					}
					else
					{
						MorseCharToDecode[y] = '*';
					}
				}
				
				y++;
			}
			
			y++;
			
			MorseCharToDecode[y] = '\0';
			
			flagCharFound = 0; 
			for (i=0; i < 43; i++)
			{
				if (strcmp(MorseCharToDecode, MORSE_TABLE[i]) == 0)
				{
					UART_PutChar(i + 0x30);
					flagCharFound = 1; 
					break;
				}
			}
			
			if (flagCharFound == 0)
			{
				UART_PutChar('#');
			}
			
			/**/
			for (i = 0; i<20;i++)
			{
				captureTable[i] = 0;
			}
			
			for (i = 0; i<8;i++)
			{
				MorseCharToDecode[i] = 0;
			}
		}
		
		if (flagSendSpaceChar != 0)
		{
			flagSendSpaceChar = 0;
			Uart_PutString(" ");
		}
		
		/*si le décodeur ne décode pas il dort*/
		/*réveil grace a pcint0*/
		if (timeSleep >= 2500000)
		{
			PCICR |= (1 << PCIE0);
			sleep_enable();
			sei();
			sleep_cpu();
			timeSleep = 0;
		}
	}
}

