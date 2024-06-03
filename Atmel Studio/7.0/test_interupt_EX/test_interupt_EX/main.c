/*
 * test_interupt_EX.c
 *
 * Created: 03/04/2023 08:56:23
 * Author : Fix-5
 *
 * détection d'interuption PCINTn, savoir quelle PCINT est utilier
 *
 *
 *
 *
 *
 
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "AVR_Uart.h"

int groupeInteruption = 0;
char UartMsg[100];
uint8_t pinDValue = 0;
uint8_t pinCValue = 0;
uint8_t pinBValue = 0;

/*valeur de PIN input*/
uint8_t valPortUniversel[] = {1,2,4,8,16,32,64,128};
	
	/*valeur PCINTn des 3 groupe differéent se répartisse sur les ports
	  Groupe 0 -> portB
	  Groupe 1 -> portC
	  Groupe 2 -> portD
	*/
	uint8_t valGrp0[] = {0,1,2,3,4,5,6,7};
	uint8_t valGrp1[] = {8,9,10,11,12,13,14};
	uint8_t valGrp2[] = {16,17,18,19,20,21,22,23};
		
/*masquage des chaque valeur INPUT*/
typedef union
{
	struct IO_INPUT
	{
		uint8_t INPUT_1		:1;
		uint8_t INPUT_2		:1;
		uint8_t INPUT_4		:1;
		uint8_t INPUT_8		:1;
		uint8_t INPUT_16	:1;
		uint8_t INPUT_32	:1;
		uint8_t INPUT_64	:1;
		uint8_t INPUT_128	:1;
	}S_INPUT;
	
	uint8_t byte;
		
}U_INPUT;

U_INPUT valueINPUT;

/*interuption pour PCINT groupe0*/
ISR(PCINT0_vect)
{
	/*récupere la valeur des l'entrée*/
	pinBValue = PINB;
	
	groupeInteruption = 1;
}

/*interuption pour PCINT groupe1*/
ISR(PCINT1_vect)
{
	/*récupere la valeur des l'entrée*/
	pinCValue = PINC;
	
	groupeInteruption = 2;
}

/*interuption pour PCINT groupe2*/
ISR(PCINT2_vect)
{
	/*récupere la valeur des l'entrée*/
	pinDValue = PIND;
	
	groupeInteruption = 3;
}

int main(void)
{
	uint8_t valPCINT = 0;
	
	/*EXTREMEMENT IMPORTANT :
	  Chaque pin utiliser pour une interuption dois êtres masquée
	  
	  ou chaque pin qui utilise une autre comunication comme PC4 et PC5
	  pour le TWI dois etres masquer pour pas influcer avec les pull-up
	  sur la carte atmega328 V1 de l'atelier ELO ou les pin du portB 
	  utiliser pour l'AVR ISP.
	*/
	DDRD = 0xF8;
	DDRB = 0xFE;
	DDRC = 0xF3;
	
	/* configure l'interruption de changement de niveau pour PCINT0*/
	PCMSK0 = 0;
	PCMSK0 |= ((1 << PCINT0)|(1 << PCINT1)|(1 << PCINT2)|(1 << PCINT3)|(1 << PCINT4));
	
	/* configure l'interruption de changement de niveau pour PCINT8,9,10 et 11*/
	PCMSK1 = 0;
	PCMSK1 |= ((1 << PCINT8)|(1 << PCINT9)|(1 << PCINT10)|(1 << PCINT11)|(1 << PCINT12));
	
	/* configure l'interruption de changement de niveau pour PCINT18*/
	PCMSK2 = 0;
	PCMSK2 |= ((1 << PCINT16)|(1 << PCINT17)|(1 << PCINT18)|(1 << PCINT19)|(1 << PCINT20));
	
	/* active les interruptions de changement de niveau pour PCINT*/
	PCICR = 0;
	PCICR |= ((1 << PCIE0) | (1 << PCIE1) | (1 << PCIE2));
	
	UART_Init();
	
	sei(); 
	
	while (1) 
	{
		/*si une interuption a était détecter*/
		if (groupeInteruption > 0)
		{
			/*sauvgarde la valeur des pin corespondant*/
			if (groupeInteruption == 1)
			{
				valueINPUT.byte = pinBValue;
			}
			if (groupeInteruption == 2)
			{
				valueINPUT.byte = pinCValue;
			}
			if (groupeInteruption == 3)
			{
				valueINPUT.byte = pinDValue;
			}
			
			/**/
			for (int i = 0; i < 8; i++)
			{
				if (valueINPUT.byte == valPortUniversel[i])
				{
					/*détecte quelle groupe d'interuption est choisi*/
					switch (groupeInteruption)
					{
						/*groupe 0*/
						case 1:
						groupeInteruption = 0;
						valPCINT = valGrp0[i];
						break;
						
						/*groupe 1*/
						case 2:
						groupeInteruption = 0;
						valPCINT = valGrp1[i];
						break;
						
						/*groupe 2*/
						case 3:
						groupeInteruption = 0;
						valPCINT = valGrp2[i];
						break;
					}
					
					/*affiche quelle PCINTn a étais activer
						
						bien sur dans se code j'utilise l'uart pour afficher
						mais dans un cas d'utilisation dans un autre programme
						l'utiliser pour détecter quelle pcint activer
						
						par exemple une varibale qui sera egale a valPCINT
						et grâce a sa faire un traitement de cette variable.
					*/
					sprintf(UartMsg, "PCINT%d*", valPCINT);
					Uart_PutString(UartMsg);
					_delay_ms(10);
					groupeInteruption = 0;
					break;
				}
			}
		}
	}
}
