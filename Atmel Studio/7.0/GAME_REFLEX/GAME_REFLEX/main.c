/*
 * GAME_REFLEX.c
 *
 * Created: 11/04/2023 16:15:43
 * Author : Fix-5
 */ 


#define F_CPU 16000000UL

#include <avr/io.h>
#include "TWI_Master.h"
#include "LCD_I2C.h"
#include "AVR_Uart.h"
#include <stdio.h>
#include <stdlib.h>

//#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdint-gcc.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#define START 0
#define STARTGAME 1
#define SCOREGAME 2

/*variable global*/
uint16_t addrEEPROM = 0x0000;
char lcdMsg[4];
uint16_t bestScore = 0;
uint8_t flagRestart = 0;

/*cette vriable est une sécuriter vus que jutilise l'ACO pour l'input capture
  au démarage du MCU (reset ou power-on) il y a une mise a un sur l'ACO
  donc il passe direct dans l'interuption cela empeche qu'au reset ou
  power-on du MCU fasse une fausse interuption pour le socre*/
uint8_t flag_secu = 0;
uint8_t WDT_flag = 0;
uint8_t flag_resetScore = 0;

/*valeur binaire pour allumer de 2 en 2 les leds*/
uint8_t ledStart [] = {0x03,0x0F,0x3F,0xFF};
uint8_t FlagGame = 0;

/* Global variable containing TOP value (memory) and capture*/
uint16_t actualScore = 0xFFFF;
uint16_t actualScoreSecondaire = 0x0000;
uint16_t firstCapture = 0;
uint16_t secondCapture = 0;

uint8_t overflowWhileMeasurement = 0;
uint8_t computeNewTimeBase = 0;

ISR(WDT_vect)
{
	if (WDT_flag == 1)
	{
		LCD_Clear();
		LCD_setCursor(0,0);
		LCD_PutString("trop lent");
		LCD_setCursor(1,0);
		LCD_PutString("temp MAX atteint");
		WDT_flag = 0;
		_delay_ms(2000);
	}
}

ISR (TIMER1_OVF_vect)
{
	if (overflowWhileMeasurement == 0)
	{
		if (firstCapture != 0x0000)
		{
			overflowWhileMeasurement = 1;
		}
		else
		{
			/**/
		}
	}
}

/*interuption capture 
  2 détection 
  la premiére par L'ACO quand les leds s'éteigne
  la deuxiéme par le flanc montant du SW*/
ISR(TIMER1_CAPT_vect)
{
	if (flag_secu >= 1)
	{
		/*debug*/
		//PORTD ^= 0x04;
		
		/* Deactivate all interrupt, must be quick */
		cli();
		
		/* Checking measurement status */
		/* if interrupt occured for starting a measurement, check for actual time */
		/* if time base was fast, new measurement could be longer --> reset OCR1A */
		/* if capture at rising edge already done, the interrupt must occure for */
		/* the falling edge --> Save ICR1 and ask for a new time base. */
		
		if (firstCapture == 0x0000)
		{
			firstCapture = ICR1;
			
			ACSR &= ~(1 << ACIC);
			
			/* Checking if a faster time base was set. */
			if (actualScore < 0xFFFF)
			{
				actualScore = 0xFFFF;
				OCR1A = 0xFFFF;
			}
		}
		else
		{
			secondCapture = ICR1;
			computeNewTimeBase = 1;
		}
		
		_delay_ms(3);
		TIFR1 |= (1 << ICF1);
		sei();
	}
	
	flag_secu++;
}

/*fonction sortie gpio pour compteur start*/
void sortie (int valeur)
{
	PORTC = valeur & 0x0F;
	PORTD = (valeur & 0xF0);
}

/*------------timer CTC input capture--------------*/
void Init_Timer (void)
{
	OCR1A = actualScore;
	TCCR1A = 0x00;
	
	TCCR1B = 0x00;
	TCCR1B |= 0b11001101;

	TIMSK1 = 0x00;
	TIMSK1 |= ((1 << TOIE1) | (1 << ICIE1));
}

/*reset le score
  le score quand il est reset il est maximum possible donc 4s
  4s car le watch dog timer reset automatiquement si le personne
  n'appuis pas pendant au bout de 4 sec*/
void reset_Score (void)
{
	bestScore = 4000;
	for (int j = 0; j < 17; j++)
	{
		eeprom_write_byte((uint8_t *)j,0);
	}
}

int main(void)
{	
	DDRC = 0x0F;
	DDRD = 0xF6;
	DDRB = 0x22;
	
	sortie(0);
	
	/*pour temp aléatoire avec fonction rand()*/
	srand(42);

	wdt_reset(); 
	WDTCSR |= (1 << WDCE) | (1 << WDE);
	WDTCSR = ((1 << WDIE) | (1 << WDE) | (1 << WDP3));
	
	Init_Timer();
	
	sei();
	
	TWIMaster_Init();
	UART_Init();
	
	/*affichage LCD*/
	LCD_Init(0x09,0x25);
	
	/*si le la pinb1 est préalablement mis a un avant de reset atmega328
	  le best score se reset*/
	if (PINB & (1 << PINB1))
	{
		reset_Score();
		LCD_setCursor(0,5);
		LCD_PutString("reset!");
		LCD_setCursor(1,5);
		LCD_PutString("score!");
		_delay_ms(2000);
		wdt_reset();
		flag_resetScore = 1;
	}
	
	LCD_Clear();
	LCD_setCursor(0,0);
	LCD_PutString("Reflex Game BOA");
	
	LCD_setCursor(1,0);
	LCD_PutString("Make a pulse :)");
	
	/*prend le best score mis en mémoire dans l'EEPROM*/
	for (int j = 0; j < 17; j++)
	{
		bestScore = bestScore + eeprom_read_byte((uint8_t *)j);
	}
	
	flag_secu = 0;
	
	/*input capture via ACO*/
	/*si ACIC = 1, ICP1 n'est pas détecter*/
	ACSR |= (1 << ACBG);
	ACSR |= (1 << ACIS1);
	
	wdt_reset();
	
    /* Replace with your application code */
    while (1) 
    {
		switch (FlagGame)
		{
		case START:
		
			if (WDT_flag == 0)
			{
				wdt_reset();
			}
		
			/*empeche que l'activation du bouton pour le lancement du jeux 
			  lance une interuption
			  et permet l'input capture par ACO*/
			ACSR |= (1 << ACIC);
			
			/*lance le jeux*/
			if (PINB & (1 << PINB0))
			{
				sei();
				
				/*affichage LCD*/
				LCD_Clear();
				LCD_setCursor(0,0);
				LCD_PutString("The Game Start !");
				
				LCD_setCursor(1,0);
				LCD_PutString("Best Score :");
				if (flag_resetScore == 0)
				{
					sprintf(lcdMsg, "%4d", bestScore);
				}
				else
				{
					sprintf(lcdMsg, "%4d", 0);
				}
				
				LCD_PutString(lcdMsg);
				
				_delay_ms(2000);
				
				wdt_reset();
				
				FlagGame = STARTGAME;
				LCD_Clear();
			}
			
			break;
			
		case STARTGAME:
			/*compteur leds*/
			/*affichage LCD*/
			LCD_setCursor(0,0);
			LCD_PutString("Act. Score :");

			LCD_setCursor(1,0);
			LCD_PutString("Best Score :");
			if (flag_resetScore == 0)
			{
				sprintf(lcdMsg, "%4d", bestScore);
			}
			else
			{
				sprintf(lcdMsg, "%4d", 0);
				flag_resetScore = 0;
			}
			
			LCD_PutString(lcdMsg);
			
			/*compteur leds*/
			for (int i = 0; i < 4; i++)
			{
				cli();
				_delay_ms(750);	
				sortie(ledStart[i]);	
				wdt_reset();		
			}
			
			sei();
			
			// Attend pendant le délai aléatoire
			/* le delai se situe entre 500ms et 2s
				
				calcule :
				prescaler/F_CPU = 1024/16000000 = 0,000064
				500ms -> 0,5/0,000064 = 7812;
				2s	  -> 2/0,000064 = 31250
			*/
			TCNT1 = 0;
			OCR1A = rand() % 31250 + 7812;
			wdt_reset();
			while(TCNT1 < OCR1A);
			/*remis valeur max OCR1A pour ravoir létat initiale du timer 
			  pour l'input capture par la suite*/
			OCR1A = 0xFFFF;
			
			WDT_flag = 1;
			wdt_reset();
			
			FlagGame = SCOREGAME;
			
			PORTC = 0;
			PORTD = 0;
			
			break;
			
		case SCOREGAME:
			 /*valeur capture
			  mise en eeprom best score
			  */
			if(computeNewTimeBase != 0)
			{
				/*empeche n'importe quelle autre appuis du SW de générer une
				  interuption
				  et active la détection par ACO pour une nouvel partie*/
				ACSR |= (1 << ACIC);
				
				if (overflowWhileMeasurement <= 1)
				{
					if (secondCapture > firstCapture)
					{
						actualScore = (secondCapture - firstCapture);
					}
					else
					{
						actualScore = ((0xFFFF - firstCapture) + secondCapture);
					}
				}
				
				/*la valeur de actualScore c'est la valeur OCR1A pour le
				  Timer, théoriquement pour savoir cette valeur si ont
				  veux par exemple 500ms sa serais
				  0,5 / 0,000064 = 7812 
				  mais vus que moi je veux le temp je fait une inverison
				  7812 * 0,000064 = 0,5 * 1000 pour 500ms*/
				actualScore = (actualScore * 0.000064) * 1000;
				
				TCNT1 = 0x0000;
				firstCapture = 0;
				secondCapture = 0;
				computeNewTimeBase = 0;
				overflowWhileMeasurement = 0;
				
				LCD_Clear();
				LCD_setCursor(0,5);
				
				/*si actualScore et plus petit que le best change le best 
				  score par l'actual et met en mémoire le best score
				  dans l'eeprom*/
				if (actualScore < bestScore)
				{
					actualScoreSecondaire = actualScore;
					
					for (int j = 0; j < 17; j++)
					{
						eeprom_write_byte((uint8_t *)j,0);
					}
					
					/*vus qu'une addresse de l'epprom peux contenir max 255
					  en décimal, je met en mémoire par packet de 255 dans
					  le nombre d'adresse nécaissaire */
					while(actualScoreSecondaire > 0x0000)
					{
						wdt_reset();
						if (actualScoreSecondaire > 255)
						{
							eeprom_write_byte((uint8_t *)addrEEPROM, 255);
							addrEEPROM++;
							actualScoreSecondaire -= 255;
						}
						else
						{
							eeprom_write_byte((uint8_t *)addrEEPROM, actualScoreSecondaire);
							actualScoreSecondaire -= actualScoreSecondaire;
						}
					}
					
					bestScore = 0x0000;
					
					/*j'additionne les valeur de chaque adresse qui comporte
					  une valeur pour avoir au total le best score */
					for (int j = 0; j < addrEEPROM + 1; j++)
					{
						bestScore = bestScore + eeprom_read_byte((uint8_t *)j);
						wdt_reset();
					}
					
					LCD_PutString("WINNER");
				}
				else
				{
					LCD_PutString("LOOSER");
				}
				
				/*affichage LCD*/
				LCD_setCursor(1,1);
				LCD_PutString("Score : ");
				
				LCD_setCursor(1,9);
				sprintf(lcdMsg, "%d", actualScore);
				LCD_PutString(lcdMsg);
				
				wdt_reset();
				_delay_ms(3000);
				wdt_reset();
				
				LCD_setCursor(0,0);
				LCD_PutString("Act. Score :");
				sprintf(lcdMsg, "%4d", actualScore);
				LCD_PutString(lcdMsg);
				
				LCD_setCursor(1,0);
				LCD_PutString("Best Score :");
				sprintf(lcdMsg, "%4d", bestScore);
				LCD_PutString(lcdMsg);
				
				cli();
				
				FlagGame = START;
			}
				
			break;
			
		default:
			/**/
			break;
			
		}
    }
}