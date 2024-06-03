/*
 * PWM_alim_decoupage_simu_AOP.c
 *
 * Created: 31.10.2023 09:00:29
 * Author : adria
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include "AVR_ADC.h"
//#include "AVR_Uart.h"
//#include "TWI_Master.h"
//#include "LCD_I2C.h"
#include <stdio.h>
//#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdint-gcc.h>
#include <util/delay.h>
//#include <avr/eeprom.h>


int main(void)
{
	uint16_t adcVal = 0;
	float tension = 0;
	char uartmsg[16];
	
	DDRC = 0x01;
	DDRD = 0x02;
	
	ADC_Init(1,128);
	UART_Init();
	
	sei();
    /* Replace with your application code */
    while (1) 
    {
		adcVal = ADC_ReadMeanValue(0x01,255);
		
		if (adcVal < 986)
		{
			PORTC = 0x01;
		}
		else
		{
			PORTC = 0x00;
		}

    }
}

