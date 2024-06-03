/*
 * WDT_test.c
 *
 * Created: 11/04/2023 09:44:01
 * Author : Fix-5
 */ 



#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdio.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdint-gcc.h>
#include <util/delay.h>


int main(void)
{
	uint8_t flag = 0;
	
	DDRC = 0x0F;
	
	MCUSR = 0;
	
	WDTCSR = 0;
	WDTCSR |= (1 << WDCE);
	
	WDTCSR &= ~(1 << WDE);
	WDTCSR &= ~(1 << WDIE);
	
	WDTCSR |= ((1 << WDP0) | (1 << WDP1) | (1 << WDP2));
	
	WDTCSR &= ~(1 << WDCE);
	
	sei();
	
    /* Replace with your application code */
    while (1) 
    {
		if (flag == 0)
		{
			PORTC ^= 0x0F;
			_delay_ms(500);
		}
		
		flag = 1;
    }
}

