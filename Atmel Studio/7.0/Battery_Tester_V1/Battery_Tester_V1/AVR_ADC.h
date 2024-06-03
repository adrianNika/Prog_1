/*
 * AVR_Uart.h
 *
 * Auteur :		David Perrin
 * Date :		22.04.2022
 * Version :	V1
 *
 * Description : 
 *		Defines, Includes & Prototypes of AVR_ADC.c
 *
 *		Comment/Uncomment following desired working mode
 *		
 *		MCU compatibility : 
 *
 *			- ATMEGA8 & ATMEGA8L
 *			- ATMEGA48A/88A/168A/328A & PA 
 *			
*/ 

#ifndef AVR_ADC_H_
#define AVR_ADC_H_

#include "global.h"

// AVOID no F_CPU issue
#ifndef F_CPU
	#define F_CPU 16000000UL
	#warning "F_CPU NOT DEFINED -> DEFINED at 16M in AVR_ADC.H"
#endif

// Utilities includes
#include <util/delay.h>
#include <stdint-gcc.h>
#include <avr/io.h>
#include <stdio.h>

// ADC Defines
#define	ADCREF_AREF			0x00	// Refering to DS, using AREF
#define ADCREF_AVCC			0x01
#define ADCREF_VREF_INT		0x03

// ADC Value corresponding to 2.56V (Internal VREF of ATMega8/8L), when VCC = 5V
#define ADC_VALUE_OF_2_56V 505	// (1024 * 2.56) / 5 = 523

// User Types


// User Prototypes
void ADC_SetPrescaler(uint8_t prescaler);
void ADC_SetReference (uint8_t refVoltage);
void ADC_Init(uint8_t vref, uint8_t prescaler);
uint16_t ADC_ReadValue(uint8_t channel);
uint16_t ADC_ReadMeanValue(uint8_t channel, uint8_t nSamples);
uint16_t ADC_ReadADCVref(uint8_t channel, uint16_t refVoltagemV);


#endif /* AVR_ADC_H_ */