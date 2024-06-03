/*
 * TIMER-0_CTC.c
 *
 * Created: 11/01/2023 11:46:57
 * Author : Fix-5
 *
 *
 *
 * Timer 0 avec 8bits
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#ifndef VALEUR_TOP
#define VALEUR_TOP 159
#endif

#ifndef TIMER_START
#define TIMER_START CS10
#endif

#ifndef TIMER_STOP
#define TIMER_STOP 0x00
#endif


void Init_Timer (void)
{
	/*F_min = 16M/256 = 62.5kHZ*/
	/*F_interupt = 100Khz*/
	
	/*Config 2 CTC avec prescaler de 1 car F_interupt > F_min */
	TCCR0A = 0x00;
	TCCR0A |= (1 << WGM01);
	
	TCCR0B = 0x00;
	TCCR0B |= (1 << CS10);
	
	/*interpution qaund compteur atteint la valeur de registre OCR0A*/
	TIMSK0 = 0x00;
	TIMSK0 |= (1 << OCIE0A);
	
	/*ICR1 => VALEUR_TOP = F_CPU / F_interupt - 1 = 16M / 100000 - 1 = 159 ticks*/
	OCR0A = VALEUR_TOP;
}

/*select prescaler de 1 => Start clock timer*/
void Timer_Start(void)
{
	TCCR0B |= TIMER_START;
}

/*NO select prescaler => NO CLOCK*/
void Timer_Stop(void)
{
	TCCR0B |= TIMER_STOP;
}

/*interuption par comparaison avec OCR0A et le compteur*/
ISR(TIMER0_COMPA_vect)
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


