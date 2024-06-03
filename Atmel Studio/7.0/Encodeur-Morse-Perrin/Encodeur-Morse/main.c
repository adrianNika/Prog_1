/*
 * MorseEncoder.cpp
 *
 * Created: 1/17/2023 1:57:47 PM
 * Author : David Perrin
 *
 *
 * Description : 
 *
 *		A vous de remplir avec votre compréhension du code.!
 *
 */ 
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint-gcc.h>
#include "AVR_Uart.h"


/* Top value for a dot @15WPM = 1200/15 = 80 [ms] = 0,08 [s]*/
/* dot duration = ((16000000 * 0.08)/1024) -1 = 1249 */
/* Dash duration = 3* dot duration = ((16000000 * 0.24)/1024) -1 = 3749  */
/* space duration = 7* dot duration = ((16000000 * 0.56)/1024)-1 = 8749 */
const uint16_t DOT_DURATION = 1249; 
const uint16_t DASH_DURATION = 3749;
const uint16_t SPACE_DURATION = 8749;

/* State machine for sending chars in morse */
typedef enum{
		STATE_ENCODER_STOPPED,
		STATE_SEND_DASH_DOT,
		STATE_DOT_DASH_SENT
	}E_STATE_ENCODER;

/* State machine variable */
E_STATE_ENCODER	stateMorseSender = STATE_ENCODER_STOPPED; 

const char *MORSE_TABLE[] = {"-----",	/* 0 */
							".----",	/* 1 */
							"..---",	/* 2 */
							"...--",	/* 3 */
							"....-",	/* 4 */
							".....",	/* 5 */
							"-....",	/* 6 */
							"--...",	/* 7 */
							"---..",	/* 8 */
							"----.",	/* 9 */
							"---...",	/* : */
							"-.-.-.",	/* ; */
							"",			/* < */
							"-...-",	/* = */	
							"",			/* > */
							"..--..",	/* ? */
							".--.-.",	/* @ */
							".-",		/* a */
							"-...",		/* b */
							"-.-.",		/* c */
							"-..",		/* d */
							".",		/* e */
							"..-.",		/* f */
							"--.",		/* g */
							"....",		/* h */
							"..",		/* i */
							".---",		/* j */
							"-.-",		/* k */
							".-..",		/* l */
							"--",		/* m */
							"-.",		/* n */
							"---",		/* o */
							".--.",		/* p */
							"--.-",		/* q */
							".-.",		/* r */
							"...",		/* s */
							"-",		/* t */
							"..-",		/* u */
							"...-",		/* v */
							".--",		/* w */
							"-..-",		/* x */
							"-.--",		/* y */
							"--.."};	/* z */

/* Global variables to be used while sending morse value */ 
uint8_t messagePointer = 0;

char stringToSend[100] = "BIENVENUE A TOUS";

uint8_t CharOfStrPtr = 0;
uint8_t morseTableCharPos = 0;
uint8_t dashDotPtr = 0;
uint8_t flagStringSent = 0;

ISR(TIMER1_COMPA_vect)
{
	/* Toggle Port automaticaly with OC1A */
	
	/* Sart Debugging */
	PORTD = 0x01;
	
	/* Enable sound, Timer 0 */
	TCCR0B ^= 0x4;
	
	/* sets position of char in morse table (offset of 0x30)*/
	morseTableCharPos = (uint8_t)((uint8_t)stringToSend[CharOfStrPtr] - 0x30);
	
	switch (stateMorseSender)
	{
		case STATE_SEND_DASH_DOT:
			/* If we have to send a char (dash or dot), set next top */
			/* value (duration) */
			if (MORSE_TABLE[morseTableCharPos][dashDotPtr] == '-')
			{
				OCR1A = DASH_DURATION;
			}
			else
			{
				OCR1A = DOT_DURATION;
			}
			
			/* On next interrupt, we check if everything is sent or not */
			stateMorseSender = STATE_DOT_DASH_SENT;
		break;
		
		case STATE_DOT_DASH_SENT:
			/* Assume we haven't sent all the dash/dots */ 
			dashDotPtr ++; 
			
			/* If all dot/dash were sent, pointed char is '\0' 
				--> select next char to be sent */ 
			if (MORSE_TABLE[morseTableCharPos][dashDotPtr] == '\0')
			{
				/* if not a dot or a dash: char sent out --> next char */
				dashDotPtr = 0;
				CharOfStrPtr ++;
				
				/* check for if a space has to be sent out */
				if (stringToSend[CharOfStrPtr] == ' ')
				{
					OCR1A = 0;
					/* BUG FOUND When multiple spaces --> should not arrive */
					/* Dummy fix -> Count number of spaces*/
					while(stringToSend[CharOfStrPtr] == ' ')
					{
						OCR1A += SPACE_DURATION;
						CharOfStrPtr ++;
					}
					
					/* @TO DO : ERROR HANDLING IF TOO MUCH SPACE 
					(65k5 / Dash_duration)*/
					stateMorseSender = STATE_SEND_DASH_DOT;				
				}
				else
				{
					/* Check if no more char has to be sent out */
					if (stringToSend[CharOfStrPtr] == '\0')
					{
						/* stops timer & clear everything */
						flagStringSent = 1;
						TCCR1B &= ~(0x07);
						stateMorseSender = STATE_ENCODER_STOPPED;
					}
					else
					{
						/* another char has to be sent out */
						OCR1A = DASH_DURATION;
						stateMorseSender = STATE_SEND_DASH_DOT;
					}
				}
			}
			else
			{
				/* another dash/ dot to be sent */
				OCR1A = DOT_DURATION;
				stateMorseSender = STATE_SEND_DASH_DOT;
			}
			
		break;
			
		default:
		break;
	}
	
	/*END Debugging*/
	PORTD = 0x00;
}

int main(void)
{
	/* Enable Timer 1 in CTC mode, OCR1A as top value */
	TCCR1A = 0; /* insure to be initialized at 0 */
	TCCR1A |= (1 << COM1A0);
	
	TCCR1B = 0; /* insure to be initialized at 0 */
	TCCR1B |= (1 << WGM12);
	
	TIMSK1 = (1 << OCIE1A);
	
	/*Timer 0 for sound*/
	TCCR0A = 0;
	TCCR0A |= ((1 << COM0A0) | (1 << WGM01));
	TCCR0B = 0;
	OCR0A = 35;  
	
	/*OC1A pin in output mode */
	DDRB |= 0x02;
	DDRD |= 0x41;
	
	/* DUMMY LOOP */
    while (1) 
    {	
		if (flagStringSent == 0)
		{
			stateMorseSender = STATE_SEND_DASH_DOT;
			
			/* activate Timer */
			OCR1A = DOT_DURATION;
			TCCR1B |= ((1 << CS12) | (1 << CS10));
			sei();
			
			/* Waiting for string to be sent, DUMMY MODE*/
			while(flagStringSent == 0);
			
			/* DUMMY RESTART */
			cli();
			CharOfStrPtr = 0;
			dashDotPtr = 0;
			morseTableCharPos = 0;
			flagStringSent = 0;
			
			_delay_ms(1000);
			stateMorseSender = STATE_SEND_DASH_DOT;
			TCCR1B |= ((1 << CS12) | (1 << CS10));
			sei();		
		}
    }
}


