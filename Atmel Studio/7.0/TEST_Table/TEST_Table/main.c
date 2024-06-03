/*
 * TEST_Table.c
 *
 * Created: 31/01/2023 13:36:53
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



int main(void)
{
	char CARACTERE_TABLE[] = {'.', '.', '\0'};
	char lettre;
	int A = 0;
	
	// Initialization of UART
	UART_Init();
	
	sei();
	
    while (1) 
    {		
		for (int i = 0; i < 43; i++)
		{
			if (strcmp(CARACTERE_TABLE, MORSE_TABLE[i]) == 0)
			{
				lettre = 0x30 + i;
				UART_PutChar(lettre);
				UART_PutChar(0x2A);
				_delay_ms(1000);
				break;
			}
		}	
		
		
    }
}
