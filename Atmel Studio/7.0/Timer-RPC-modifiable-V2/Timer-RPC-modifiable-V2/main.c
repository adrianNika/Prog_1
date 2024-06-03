/*
 * Timer-RPC-modifiable-V2.c
 *
 * Created: 10/01/2023 14:00:16
 * Author : Fix-5
 */ 

#define F_CPU 16000000UL /*fréquence du MCU*/
#include <avr/io.h>


/*fonction timer mode fast PWM ICR1*/
void Init_Timer(void)
{
	/*COM1A1 actif => OCA1 en sortie
	  WGM11 actif => mode PWM ICR1*/
			TCCR1A |= ((1<<COM1A1) | (1<<WGM11));
													
	/*WGM12 et WGM13 actif => mode PWM ICR1
	  CS10 actif => select prescaler de 1 */	
			TCCR1B |= ((1<<WGM13) | (1<<WGM12) | (1<<CS10)); 
	
	/*valeur TOP max avec Fréquence de 42KHz et prescaler de 1*/
			ICR1 = 380; 
	
	/*PINB1 actif => OCA1 sortie*/
			DDRB |= (1<<PINB1); 
}

int main(void)
{
	/*initialiser la fonction timer*/
	Init_Timer();
	
	/*initialiser les entrees*/
	DDRD = 0x00; 
	
	uint8_t entree = 0;
	uint8_t lastEntree = 0;
	uint8_t rapport_cyclique= 0;
	
	while (1)
	{
		/*valeur Rapport cycliquer simuler*/
		entree = PIND;
		
		/*calcule rpc et assigne la valeur => OCR1A*/
		if (lastEntree != entree)
		{
			/*asigne la valeur du rapport cyclique*/
			lastEntree = entree;
			rapport_cyclique = lastEntree;
			
			/*calculer la valeur OCR1A avec le rpc(rapport cyclique)*/
			OCR1A = rapport_cyclique * ICR1 / 100; 
		}
	}
}

