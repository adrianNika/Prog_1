/*
 * Temperature_ADC_NTC_V2.c
 *
 * Created: 14/02/2023 10:32:48
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


int temperature[] = {  -55,
						-50,
						-45,
						-40,
						-35,
						-30,
						-25,
						-20,
						-15,
						-10,
						-5,
						0,
						5,
						10,
						15,
						20,
						25,
						30,
						35,
						40,
						45,
						50,
						55,
						60,
						65,
						70,
						75,
						80,
						85,
						90,
						95,
						100,
						105,
						110,
						115,
						120,
						125};
						
int adcTemp[] = { 11,
					16,
					22,
					31,
					42,
					57,
					76,
					99,
					127,
					160,
					199,
					243,
					292,
					344,
					399,
					455,
					511,
					567,
					618,
					666,
					710,
					750,
					785,
					817,
					844,
					868,
					888,
					906,
					922,
					935,
					946,
					956,
					964,
					971,
					978,
					983,
					988};


int main(void)
{
	uint16_t adc = 0;
	uint16_t adcMin = 0;
	uint16_t adcMax = 0;
	uint16_t tempMin = 0;
	uint16_t tempMax = 0;
	uint16_t temperatureValue = 0;
	char uartMsg[16];
	
	ADC_Init(ADCREF_AVCC,128);
	UART_Init();
	
	sei();
	
    while (1) 
    {
		adc = ADC_ReadMeanValue(0,255);
		
		for (int i = 0; i < 38; i++)
		{
			if (adc <= adcTemp[i])
			{
				adcMax = adcTemp[i];
				tempMax = temperature[i];
				adcMin = adcTemp[i-1];
				tempMin = temperature[i-1];
				
				temperatureValue = ((adcMax - adcMin) / (tempMax - tempMin)) + 25;
				break;
			}
		}
		sprintf(uartMsg, "ADC = %04d   ", adc);
		Uart_PutString(uartMsg);
		sprintf(uartMsg, "T = %03d C;", temperatureValue);
		Uart_PutString(uartMsg);
		_delay_ms(1000);
		temperatureValue = 0;
    }
}

