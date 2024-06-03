/*
 * Timer-0_PWM.c
 *
 * Created: 11/01/2023 13:07:37
 * Author : Fix-5
 */ 

#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include "AVR_ADC.h"
#include "AVR_Uart.h"
#include "global.h"

#define F_CPU 16000000UL   /*Fréquence d'horloge du microcontrôleur (16 MHz)*/
#define F_INTERRUPT 70000  /*Fréquence d'interruption souhaitée*/
#define DUTY_CYCLE 50    /*Rapport cyclique en pourcentage d'initialisation de l'alimentation*/

/*initialisation variable pour timer*/
uint16_t ICR1_value = 0;
uint16_t OCR1A_value = 0;

void Init_TIMER (void)
{
	 /*Configuration des bits WGM pour le mode Fast PWM, avec TOP = ICR1*/
	 TCCR1A |= (1 << WGM11);
	 TCCR1B |= (1 << WGM12) | (1 << WGM13);

	 /*Configuration du prescaler à 1 (pas de division)*/
	 TCCR1B |= (1 << CS10);

	 /*Calcul de la valeur de ICR1 (TOP) pour obtenir la fréquence d'interruption souhaitée*/
	 ICR1_value = F_CPU / (F_INTERRUPT) - 1;
	 ICR1 = ICR1_value;

	 /*Calcul de la valeur de comparaison (OCR1A) pour obtenir le rapport cyclique souhaité*/
	 OCR1A_value = (DUTY_CYCLE * ICR1_value) / 100;
	 OCR1A = OCR1A_value;

	 /*Configuration du bit COM1A1 pour activer la sortie PWM sur la broche OC1A*/
	 TCCR1A |= (1 << COM1A1);

	 /*Configuration de la broche OC1A comme sortie*/
	 DDRB |= (1 << DDB1);
}

int main(void)
{
	DDRB = 0x00;
	uint8_t PWM_variable = 50;
	
	/*initialise le timer et le start*/
	Init_TIMER();
    
    while (1) 
    {
		/*si sortie aop a 1 sur PINC0 augmente le PWM pour stabiliser la tension*/
		if (PINC & (1 << PINC0))
		{
			if (PWM_variable < 99)
			{
				PWM_variable++;
			}
		}
		/*si sortie aop a 0 sur PINC0 diminue le PWM pour stabiliser la tension*/
		else
		{
			if (PWM_variable > 1)
			{
				PWM_variable--;
			}
		}
		
		/*nouveau rapport cyclique pour le timer*/
		uint16_t OCR1A_value = (PWM_variable * ICR1_value) / 100;
		OCR1A = OCR1A_value;
		_delay_ms(1);
    }
}