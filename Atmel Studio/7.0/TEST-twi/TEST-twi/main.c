/*
 * TEST-twi.c
 *
 * Created: 23.11.2023 14:14:37
 * Author : adria
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>

#include "AVR_ADC.h"
#include "AVR_Uart.h"
#include "TWI_Master.h"
#include "AVR_LCD_1602_I2C.h"

#include <stdio.h>
//#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdint-gcc.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <string.h>


uint16_t tON = 0;
uint8_t enter_shortPulse = 0;
uint8_t enter_longPulse = 0;

ISR (PCINT2_vect)
{
	enter_shortPulse = 1;
	/*if (PIND & (1 << PIND5))
	{
		if (tON < 400 && tON > 0)
		{
			enter_shortPulse = 1;
		}
		else if (tON > 400)
		{
			enter_longPulse = 1;
		}
		
		TCCR1B &= ~(1 << CS10);
		TCNT1 = 0;
	}


	if (PIND & ~(1 << PIND5))
	{
		TCCR1B |= (1 << CS10);
	}*/
	
	/*if (PIND & ~(1 << PIND5))
	{
		enter_shortPulse = 1;
	}*/
}

ISR (TIMER1_COMPA_vect)
{
	tON++;
}

void init_interPCINT(void)
{
	PCMSK2 |= ((1 << PCINT19)|(1 << PCINT20)|(1 << PCINT21)|(1 << PCINT22)|(1 << PCINT23));
	PCICR |= (1 << PCIE2);
}

void init_Timer1(void)
{
	TCCR1A |= 0x00;
	TCCR1B |= (1 << WGM12);
	TIMSK1 |= (1 << OCIE1A);
	OCR1A = 7999;
}

int main(void)
{
	DDRD = 0b00000110;
	
	/* Start-up delay */
	_delay_ms(100);
	
	/*Structure for LCD Data*/
	S_LCD_DATA lcd = {  .i2c.S_MsgSplit.SlaveAdress = 0x27,
						.i2c.S_MsgSplit.ReadWriteBit = TWI_WRITE,
						.cursorX = 0x00,
						.cursorY = 0x00,
						.tableIndex = 0x00,
						.appendMode = 0x01,
						.error = 0x00,
						.pcfdata.reg.backlight = 0x01};

	sei();
	
	init_interPCINT();
	init_Timer1();
	
	/* Initialize ADC + Uart */
	ADC_Init(1,128);
	UART_Init();

	/* Initialize I2C + LCD */
	TWIMaster_Init();
	LCD_Init(&lcd);
	
	/*start Battery tester with title*/
	LCD_SetCursor(&lcd, 0, 1);
	LCD_PutString(&lcd,"Battery Tester");
	LCD_SetCursor(&lcd, 1, 7);
	LCD_PutString(&lcd, "V1");
	PORTD = 0x04;
	_delay_ms(2000);
	PORTD = 0x00;
	LCD_Clear(&lcd);
	LCD_SetCursor(&lcd, 0, 0);
	
	while (1)
	{
		/*if (enter_shortPulse == 1)
		{
			LCD_Clear(&lcd);
			LCD_SetCursor(&lcd, 0, 0);
			enter_shortPulse = 0;
			LCD_PutString(&lcd, "S");
		}
		if (enter_longPulse == 1)
		{
			LCD_Clear(&lcd);
			LCD_SetCursor(&lcd, 0, 0);
			enter_longPulse = 0;
			LCD_PutString(&lcd, "L");
		}*/
		
		if (enter_shortPulse == 1)
		{
			LCD_PutString(&lcd, "L"); 
			while(1);
		}
	}
}


