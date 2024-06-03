/*
 * Decoder_Morse-V2.c
 *
 * Created: 02/01/2023 09:37:28
 * Author : Fix-5
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint-gcc.h>
#include <util/delay.h>
#include <string.h>
#include "global.h"
#include "AVR_Uart.h"
#include "LCD_I2C.h"
#include "TWI_Master.h"


/*global variable*/
uint16_t morseValue = 0xFFFF;
uint16_t morseValueBas = 0x0000;
uint16_t morseValueHaut = 0x0000;

uint16_t firstCapture = 0x0000;
uint16_t SecondCapture = 0x0000;

uint8_t computeNewTimeMorse = 0;
uint8_t flag = 0;
uint8_t overFlowWhileMeasurement = 0;
uint8_t Stop_init_high_value = 0;
uint8_t compteur = 0;

char element;
char CARACTERE_TABLE[] = {0};

char lettre;
uint8_t j = 0;



/* Top value for a dot @15WPM = 1200/15 = 80 [ms] = 0,08 [s]*/
/* dot duration = ((16000000 * 0.08)/1024) -1 = 1249 */
/* Dash duration = 3* dot duration = ((16000000 * 0.24)/1024) -1 = 3749  */
/* space duration = 7* dot duration = ((16000000 * 0.56)/1024)-1 = 8749 */

const uint16_t DOT_DURATION_LOW = 1100;
const uint16_t DOT_DURATION_HIGH = 1400;
const uint16_t DASH_DURATION_LOW = 3200;
const uint16_t DASH_DURATION_HIGH = 4000;
const uint16_t SPACE_DURATION = 8749;

/* State machine for sending chars in morse */
typedef enum{
	STATE_DECODER_STOPPED,
	STATE_SPACE_TIME,
	STATE_CONVERTION
}E_STATE_DECODER;

/* State machine variable */
E_STATE_DECODER stateMorseReceiver = STATE_DECODER_STOPPED;



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

ISR (TIMER1_OVF_vect)
{
	compteur += 1;
	
	if (overFlowWhileMeasurement == 0)
	{
		if (firstCapture != 0x0000)
		{
			overFlowWhileMeasurement++;
		}
	} 
	else
	{
		/**/
	}
}

ISR (TIMER1_CAPT_vect)
{
	cli();
	
	if (firstCapture == 0x0000)
	{
		firstCapture = ICR1;
		
		if (flag == 1)
		{
			morseValueBas = firstCapture - SecondCapture;
		}

		if (morseValueHaut < 0xFFFF)
		{
			morseValueHaut = 0xFFFF;
			OCR1A = 0xFFFF;
		}
	}
	else
	{ 
		flag = 0;
		SecondCapture = ICR1;
		computeNewTimeMorse = 1;
	}
	
	_delay_ms(2);
	TCCR1B ^= (1 << ICES1);
	TIFR1 &= ~(1 << ICF1);
	sei();
}

/*initialisation du timer*/
void Init_Timer1 ()
{
	/*Setting timer 1 in CTC mode 4, OCR1A top, starting at 0xFFFF*/
	/*no output OC1A for this application*/
	/*(TCCR1B) activate ICP1 filtering, rising edge, prescaler at 1024*/
	OCR1A = morseValue;
	
	TCCR1A = 0x00;
	//TCCR1A |= (1 << COM1A0);
	
	TCCR1B = 0x00;
	TCCR1B |= 0b11001000;
	

	/*activate two interupt (overFlow and capt)*/ 
	TIMSK1 = 0x00;
	TIMSK1 |= ((1 << TOIE1) | (1 << ICIE1));
	
	sei();
}



/* Function for start and stop Timer 1*/
void Start_Timer1()
{
	TCCR1B |= ((1 << CS12) | (1 << CS10));
}
void Stop_Timer1()
{
	TCCR1B &= ~((1 << CS12) | (1 << CS10));
}



int main(void)
{
	sei();
	DDRB |= 0x02;
	DDRC = 0x01;
	DDRD = 0x02;
	
	Init_Timer1();
	Start_Timer1();
	
	// Initialization of UART
	UART_Init();
	
	TWIMaster_Init();
	
	LCD_Init(0x0F, 0x25);
	LCD_setCursor(0,0);
	
    while (1) 
    {
		if (computeNewTimeMorse == 1)
		{
			cli();
			
			if (overFlowWhileMeasurement <= 1)
			{
				if (SecondCapture > firstCapture)
				{
					morseValueHaut = SecondCapture - firstCapture;
					
					flag = 1;
				}
				else
				{
					morseValueHaut = (0xFFFF - firstCapture) + SecondCapture;
				}
			}
			
			firstCapture = 0x0000;
			computeNewTimeMorse = 0;
			overFlowWhileMeasurement = 0;
			
			sei();
		}
		
		if (morseValueHaut > DOT_DURATION_LOW && morseValueHaut < DOT_DURATION_HIGH)
		{
			element = '.';
			CARACTERE_TABLE[j] = element;
			j++;
			morseValueHaut = 0x00;
		}
		else if (morseValueHaut > DASH_DURATION_LOW && morseValueHaut < DASH_DURATION_HIGH)
		{
			element = '-';
			CARACTERE_TABLE[j] = element;
			j++;
			morseValueHaut = 0x00;	
		}
		
		
		if (morseValueBas > 2500)
		{
			element = '\0'; 
			CARACTERE_TABLE[j] = element;
			
			for (int i = 0; i < 43; i++)
			{
				if (strcmp(CARACTERE_TABLE, MORSE_TABLE[i]) == 0)
				{
					lettre = 0x30 + i;
					LCD_PutChar(lettre);
					j = 0x00;
					break;
				}
			}
			
			morseValueBas = 0x00;
		}
    }
}

