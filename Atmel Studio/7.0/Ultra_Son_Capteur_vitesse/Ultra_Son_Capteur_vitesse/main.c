/*
 * Ultra_Son_Capteur_vitesse.c
 *
 * Created: 22.02.2024 10:50:06
 * Author : adria
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include "AVR_Uart.h"
#include "TWI_Master.h"
//#include "LCD_I2C.h"
#include "AVR_LCD_1602_I2C.h"
#include "UltrasonHCSR04.h"
#include <stdio.h>
//#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdint-gcc.h>
#include <util/delay.h>
//#include <avr/eeprom.h>

float vitesse_son = 340.29;
float time_high_lvl_1 = 0;
float time_high_lvl_2 = 0;
float distance = 0;
uint8_t flag = 1;
uint8_t startTimer = 0;
char uartstr[16];


/*initialisation du timer*/
void Init_Timer1 ()
{
	// Initialisation du Timer 1 en mode CTC
	TCCR1A = 0; // Mode normal de fonctionnement pour le Timer 1
	TCCR1B = (1 << WGM12); // Mode CTC (Clear Timer on Compare Match) avec OCR1A comme valeur de comparaison
	
	OCR1A = 159; 

	// Activation de l'interruption pour le Timer 1 (Match A)
	TIMSK1 |= (1 << OCIE1A);
	
	sei();
}

/* Function for start and stop Timer 1*/
void Start_Timer1()
{
	TCCR1B |= (1 << CS10);
}
void Stop_Timer1()
{
	TCCR1B &= ~(1 << CS10);
}


int main(void)
{
	DDRC = 0x05;
	DDRD = 0x02;
	
	PCMSK1 |= (1 << PCINT9);
	PCMSK2 |= (1 << PCINT11);
	PCICR |= ((1 << PCIE1) | (1 << PCIE2));
	
	/* Start-up delay */
	_delay_ms(100);
	
	/* Structure for LCD Data */
	S_LCD_DATA lcd = {  .i2c.S_MsgSplit.SlaveAdress = 0x27,
						.i2c.S_MsgSplit.ReadWriteBit = TWI_WRITE,
						.cursorX = 0x00,
						.cursorY = 0x00,
						.tableIndex = 0x00,
						.appendMode = 0x01,
						.error = 0x00,
					    .pcfdata.reg.backlight = 0x01};

	sei();

	/* Initialize I2C + LCD */
	TWIMaster_Init();
	LCD_Init(&lcd);
	
	Init_Timer1();
	
	UART_Init();
	
    /* Replace with your application code */
    while (1) 
    {	
		if (flag == 1)
		{	
			time_high_lvl_1 *= 10;
			time_high_lvl_1 *= 0.017;
			time_high_lvl_2 *= 10;
			time_high_lvl_2 *= 0.017;
			
			/*sprintf(uartstr, "%.1f cm;", time_high_lvl);*/
			/*Uart_PutString(uartstr);*/
			
			LCD_Clear(&lcd);
			
			sprintf(uartstr, "%.2f cm", time_high_lvl_1);
			LCD_SetCursor(&lcd,0,0);
			LCD_PutString(&lcd,uartstr);
			
			sprintf(uartstr, "%.2f cm", time_high_lvl_2);
			LCD_SetCursor(&lcd,1,0);
			LCD_PutString(&lcd,uartstr);
			
			_delay_ms(250);
			
			flag = 0;
			
			PORTC |= (1 << PORTC2);
			_delay_us(10);
			PORTC &= ~(1 << PORTC2);
			PORTC |= (1 << PORTC0);
			_delay_us(10);
			PORTC &= ~(1 << PORTC0);
		}
    }
}


ISR(PCINT1_vect)
{
	startTimer ^= 1;
	
	if (startTimer == 1)
	{
		Start_Timer1();
	}
	else
	{
		Stop_Timer1();
		flag = 1;
	}
	
}

ISR(PCINT2_vect)
{
	startTimer ^= 1;
	
	if (startTimer == 1)
	{
		Start_Timer1();
	}
	else
	{
		Stop_Timer1();
		flag = 1;
	}
	
}

ISR(TIMER1_COMPA_vect)
{
	time_high_lvl_1++;
	time_high_lvl_2++;
	
	TCNT1 = 1;
}