/*
 * Timer-cligno.c
 *
 * Created: 12/01/2023 13:57:18
 * Author : Fix-5
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef TIMER_START
#define TIMER_START 0x01
#endif

#ifndef TIMER_STOP
#define TIMER_STOP 0x00
#endif

uint16_t val = 0;
uint16_t flag = 0;

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

void Init_Timer(void)
{
	TCCR1A |= ((1 << COM1B1) | (1 << WGM11) | (1 << WGM10));
	
	TCCR1B |= ((1 << WGM13) | (1 << WGM12));
	TCCR1B |= (1 << ICNC1);
	TCCR1B |= (1 << ICES1);
	
	TIFR1 |= (1 << ICF1);
	TIFR1 |= (1 << TOV1);
	
	TIMSK1 |= (1 << ICIE1);
	TIMSK1 |= (1 << TOIE1);
	
	ACSR |= (1 << ACIC);
	
	OCR1A = ICR1;
	OCR1B = 0;
	
	PORTD = 0x04;
}

ISR(TIMER1_OVF_vect)
{
	if (ICR1 > 0)
	{
		if (val < ICR1 && flag == 0)
		{
			val++;
			OCR1B = val;
		}
		else if (val > ICR1 && flag == 1)
		{
			val--;
			OCR1B = val;
		}
		
		if (val == ICR1)
		{
			flag = 1;
		}
		if (val == 1)
		{
			flag = 0;
		}
	}
}


int main(void)
{
	DDRB = 0x04;
	
	Init_Timer();
	
	Timer_Start();
	
	sei();
	
	while (1)
	{
		if(bit_is_set(PINB,0))
		{
			;
		}
	}
}
