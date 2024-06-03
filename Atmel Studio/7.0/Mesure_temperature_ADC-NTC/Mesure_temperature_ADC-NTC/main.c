/*
 * Mesure_temperature_ADC-NTC.c
 *
 * Created: 07/02/2023 15:45:32
 * Author : Fix-5
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include "AVR_ADC.h"
#include "AVR_Uart.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint-gcc.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

int main(void)
{
	DDRC = 0x00;
	
	DDRD = 0x04;
	
	float R25 = 4700;
	float R0 = 9700;
	float B = 3950;
	float T0 = 298.15;
	char uartMsg[16];
	float tempc = 0;
	uint16_t adc = 0;
	uint16_t adc_flag = 0;
	float value_1 = 1;
	
	UART_Init();
	ADC_Init(ADCREF_AVCC, 128);
	
	sei();
	
    while (1) 
    {
		adc = ADC_ReadMeanValue(1, 255);
		 
		tempc = value_1 / ((value_1 / T0) + (value_1 / B) * log((R0 / R25) * (((float)1024 / adc) - value_1))) - 273.15;
		
		if (adc_flag != adc)
		{
			sprintf(uartMsg, "ADC =%4d   ", adc);
			Uart_PutString(uartMsg);
			sprintf(uartMsg, "T = %.1f C;",tempc);
			Uart_PutString(uartMsg);
			_delay_ms(400);
		}
		
		adc_flag = adc;
    }
}

