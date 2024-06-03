/*
 * Timer-Mode-CTC.c
 *
 * Created: 01/01/2009 00:14:45
 * Author : Fix-5
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#ifndef VALEUR_TOP
#define VALEUR_TOP 15999
#endif

#ifndef TIMER_START
#define TIMER_START 0x01
#endif

#ifndef TIMER_STOP
#define TIMER_STOP 0x00
#endif


void Init_Timer (void)
{
	/*F_min = 16M/65536 = 244.14Hz*/
	/*F_interupt = 1Khz*/
	
	/*Config 4 CTC avec prescaler de 1 car F_interupt > F_min */
	TCCR1B = 0x00;
	TCCR1B |= (1 << WGM12);
	
	/*interpution qaund compteur atteint la valeur de registre OCR1A*/
	TIMSK1 = 0x00;
	TIMSK1 |= (1 << OCIE1A);
	
	/*ICR1 => VALEUR_TOP = F_CPU / F_interupt - 1 = 16M / 1000 - 1 = 15999 ticks*/
	OCR1A = VALEUR_TOP;
}

/*select prescaler de 1 => Start clock timer*/
void Timer_Start(void)
{
	TCCR1B |= TIMER_START;
}

/*NO select prescaler => NO CLOCK*/
void Timer_Stop(void)
{
	TCCR1B |= TIMER_STOP;
}

/*interuption par comparaison avec OCR1A et le compteur*/
ISR(TIMER1_COMPA_vect)
{
	/*toggle de la sortie PD0*/
	PORTD ^= 0x01;
}


int main(void)
{
	DDRD = 0xFF; /*initialisation en sortie du PORTD*/
	
	Init_Timer(); /*initialiser mon timer*/
	Timer_Start();
	
	sei(); /*innitialiser les interuptions*/
    
    while (1) 
    {
		/*toggle de la sortie PD1*/
		PORTD ^= 0x02;
		_delay_ms(500);
    }
}

