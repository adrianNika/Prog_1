/*
 * Timer-0_PWM.c
 *
 * Created: 11/01/2023 13:07:37
 * Author : Fix-5
 */ 

#define F_CPU 16000000UL	
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

#define VALEUR_TOP 227 /*F_CPU / F_interupt - 1 = 16M / 256 - 1 = 227*/
#define VALEUR_COMP 57 /*Rapport cyclique * valeur_top / 100 = 25 * 227 / 100 = 57*/

void Init_TIMER (void)
{
	/*F_MIN = F_CPU / 256 = 16M / 256 = 62.5KHz*/
	/*F_interupt = 70KHz*/
	
	/*CONFIG 7 pour fast PWN avec prescaler de 1 car f_interput > F_MIN*/
	/*bit COM0B1 du registre TCCR0A active sortie OC0B => sortie avec PIND5*/
	TCCR0A = 0x00;
	TCCR0A |= ((1 << COM0B1) | (1 << WGM01) | (1 << WGM00));
	
	/* suite pour activer la CONFIG 7 pour fast PWN*/
	TCCR0B = 0x00;
	TCCR0B |= (1 << WGM02);
	
	/*OCR0A => valeur TOP*/
	OCR0A = VALEUR_TOP;
	
	/*OCR0B => valeur COMP*/
	OCR0B = VALEUR_COMP;
	
	//active portd pin5 pour la sortie OC0B
	DDRD |= (1 << PIND5);
	
}

/*Start le timer*/
void Timer_0_START (void)
{
	TCCR0B |= (1 << CS00);
}
/*stop le timer*/
void Timer_0_STOP (void)
{
	TCCR0B = 0x00;
}

int main(void)
{
	/*initialise le timer et le start*/
	Init_TIMER();
	Timer_0_START();
    
    while (1) 
    {
		;
    }
}

