/*
 * InputCapture.c
 *
 * Created: 12/01/2023 4:11:27 PM
 * Author : D.Perrin
 *
 * Solution de l'exercice "Input Capture" 
 *
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint-gcc.h>

/* Global variables for input capture */
uint16_t actualTopValue = 0xFFFF; 
uint16_t firstCapture = 0;
uint16_t secondCapture = 0;

uint8_t overflowWhileMeasurement = 0; 
uint8_t computeNewTimeBase = 0;

/* Interrupt to count if there is a longer pulse than Timer period */
ISR(TIMER1_OVF_vect)
{
	/* Checking if overflow occured while measuring a pulse */
	if (overflowWhileMeasurement == 0)
	{
		if (firstCapture != 0x0000)
		{
			overflowWhileMeasurement = 1;
		}
	}
	else
	{
		/* Error handling, 2 overflows while a capture is not supported */
	}
}

ISR(TIMER1_CAPT_vect)
{
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
		
		/* Checking if a faster time base was set. */
		if (actualTopValue < 0xFFFF)
		{
			actualTopValue = 0xFFFF;
			OCR1A = 0xFFFF; 
		}		
	}
	else
	{
		secondCapture = ICR1;
		computeNewTimeBase = 1; 
	}
	
	/* Change edge for next measurement ! */
	TCCR1B ^= (1 << ICES1);
	sei();
}


int main(void)
{
    /* Setting up Timer 1 in CTC Mode 4, OCR1A TOP, starting at 0xFFFF */
	/* Activate toggle of OC1A --> Led blinks while capture duration */
	/* (TCCR1B) activate ICP1 Filtering, rising edge, prescaler at  1024,*/

	OCR1A = actualTopValue; 
	TCCR1A = 0x00; 
	TCCR1A |= (1 << COM1A0);
	TCCR1B = 0x00;
	TCCR1B |= 0b11001101;
	
	/* GPIO must be set to generate a signal */
	DDRB |= 0x02;
	
	/* The two interrupts activated */
	TIMSK1 = 0x00; 
	TIMSK1 |= ((1 << TOIE1) | (1 << ICIE1));
	
	sei(); 
	
    while (1) 
    {
		/* Only compute new time base when measuremnet completed */
		if (computeNewTimeBase != 0)
		{
			/* Don't want an interrupt occuring now & here ! */
			cli();
			
			/* Only compute new time base if only ONE overflow occured  */
			/* while measuring the pulse. If so, compute the difference */
			/* between the two times, becoming the new top value */
			
			if (overflowWhileMeasurement <= 1)
			{
				if (secondCapture > firstCapture)
				{
					actualTopValue = secondCapture - firstCapture;
				}
				else
				{
					actualTopValue = (0xFFFF - firstCapture) + secondCapture; 
				}
			}
			
			/* Reset everything  */
			OCR1A = actualTopValue; 
			TCNT1 = 0x0000;
			firstCapture = 0x0000;
			secondCapture = 0x0000; 
			computeNewTimeBase = 0; 
			overflowWhileMeasurement = 0;
			sei();
		}
    }
}

