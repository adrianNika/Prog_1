/*********************************************************************************
*	@file		AVR_ADC.c
*	@brief		Contains ADC handling functions
*
*	Simple ADC library for AVR : 
*	- ATMEGA8 et ATMEGA8L
*	- ATMEGA48A/88A/168A/328A /PA
*
*	-File:		AVR_ADC.c
*	-Compiler:	AVR-GCC
*	-MCU:		ATMEGA8, ATMEGA88,328,etc

*	@Author		David Perrin
*	@version	1.0 
* 	@date		5/20/2022
*********************************************************************************/

#include "AVR_ADC.h"

/**
 * @brief Sets ADC clock prescaler value
 *
 * This function sets the prescaler value for ADC clock frequency.  
 * Clock frequency should be 50kHz and 200kHz. An ADC conversion 
 * Take approximatively 13 tick from ADC clock.
 * This function modify ADCSRA register. 
 *
 * @param  prescaler prescaler value(0,2,4,8,16,32,64,128)
 * @return void
 */

void ADC_SetPrescaler(uint8_t prescaler)
{
	// Prescaler should be chosen refering F_CPU
	// ADC clock should be between 50kHz...200kHz
	// Prescaler divides F_CPU by prescaler factor
	//
	// If F_CPU = 1MHz	-> MIN. presacaler = 8
	// If F_CPU = 8MHz	-> Min. prescaler = 64
	// If F_CPU = 16MHz	-> Min. prescaler = 128
	
	
	// Starting by clearing old prescaler factor
	// ADCSRA = |ADEN|ADSC|ADATE|ADIF|ADIE|ADPS2|ADPS1|ADPS0|
	//										 ^.....^.....^... <- Set to 0
	ADCSRA &= (0xF8);
	
	// Setting prescaler , following Datasheet value of registers 
	switch (prescaler)
	{
		case 0:
		// do nothing more
		break;
		
		case 2:
		ADCSRA |= 0x01;
		break;
		
		case 4:
		ADCSRA |= 0x02;
		break;
		
		case 8:
		ADCSRA |= 0x03;
		break;
		
		case 16:
		ADCSRA |= 0x04;
		break;
		
		case 32:
		ADCSRA |= 0x05;
		break;
		
		case 64:
		ADCSRA |= 0x06;
		break;
		
		case 128:
		ADCSRA |= 0x07;
		break;
		
		default:
		ADCSRA |= 0x07; // slow, but works better
		break;
	}
}

/**
 * @brief Set reference voltage for ADC
 * 
 * This function sets the input reference for ADC
 *
 * !!! Internal Reference differs between ATmega8/8L and 48/88/168/328	
 *		ATmega8/8L :			VREF = 2.56V
 *		ATmega48/88/168/328 :	VREF = 1.1V
 *
 * @param  refVoltage reference voltage (see def in AVR_ADC.h)
 * @return void
 */
void ADC_SetReference (uint8_t refVoltage)
{
	// Selecting Voltage reference for ADC
	// 0 = ref -> AREF (internal Vref turned off)
	// 1 = ref -> AVcc (!! Put 100nF from AREF to GND !!)
	// 3 = ref -> VrefInt (!! Put 100nF from AREF to GND !!)
	
	
	// Starting by clearing old reference voltage
	// ADMUX = |REFS1|REFS0|ADLAR| - |MUX3|MUX2|MUX1|MUX0|
	//			  ^.....^.....<- Set to 0
	ADMUX &= 0x3F;
	
	switch (refVoltage)
	{
		case ADCREF_AREF:
		// nothing to do
		break;
		
		case ADCREF_VREF_INT:
		ADMUX |= 0xC0;
		break;
		
		case ADCREF_AVCC:
		default:
		ADMUX |= 0x40;
		break;
	}
	_delay_ms(25);
}

/**
 * @brief Initialization of ADC
 *
 * This Function enables the ADC, Initiate with a call to SetPRescaler
 * & SetReference. 
 *
 * ONLY for ATMEga88 family : Disable input digital buffer (no need when
 * using ADC) 
 *
 * @param  vref Reference voltage to set-up initialy (see def. in AVR_ADC.h)
 * @param  prescaler prescaler value for ADC clock (0,2,4,8,16,32,64,128)
 * @return void
 */
void ADC_Init(uint8_t vref, uint8_t prescaler)
{
	// Activate ADC periphal
	ADCSRA |= (1 << ADEN);
	
	//	Deactivate "align left" (more simple to handle)
	// if ADLAR = 0 :
	//		ADCH = |  - |  - |  - |  - |  - |  - |ADC9|ADC8|
	//		ADCL = |ADC7|ADC6|ADC5|ADC4|ADC3|ADC2|ADC1|ADC0|
	//
	// if ADLAR = 1 :
	//		ADCH = |ADC9|ADC8|ADC7|ADC6|ADC5|ADC4|ADC3|ADC2|
	//		ADCL = |ADC1|ADC0|  - |  - |  - |  - |  - |  - |
	// (use ADLAR = 1, if 2 LSB are not significant)
	ADMUX &= ~(1<<ADLAR);
	
	// Disable Digital input buffer
	// DIDR0 |= 0x0F;
	
	// Set ref Voltage
	ADC_SetReference(vref);
	
	// Set prescaler value
	ADC_SetPrescaler(prescaler);
}

/**
 * @brief read ADC value on desired channel 
 *
 * Proceed to a single ADC measure. 
 * Function is blocking -> 1/(13*(F_CPU/PRESCALER)) = ~0.2ms @CPU = 8M
 *
 * @param  channel ADC channel of MCU (0-5 on DIP, 0-7 on TQFP)
 * @return uint16_t ADC value measured (0-1023)
 */
uint16_t ADC_ReadValue(uint8_t channel)
{
	// Clear old channel selection
	// ADMUX = |REFS1|REFS0|ADLAR| - |MUX3|MUX2|MUX1|MUX0|
	//								   ^.....^....^....^ <- Set to 0
	ADMUX &= 0xF0;
	
	// Set Channel
	ADMUX |= (channel & 0x0F);
	
	// Start ADC Conversion
	ADCSRA |= (1<<ADSC);
	
	// BLOQUING : WAIT END OF CONVERSION
	// To do : Handling with interrupt
	while((ADCSRA & (1<<ADIF))==0);
	
	return (uint16_t)(ADCL | (ADCH << 8));
}


/**
 * @brief Proceed to ADC measurement and return the average of nSamples
 * 
 * Proceed to a Average of "0 to 255" Samples
 * Function is blocking : takes up to 6ms for nSample = 255
 *
 *@param  channel ADC channel where to measure
 * @param  nSamples Number of sample to average
 * @return uint16_t ADC value, averaged 
 */
uint16_t ADC_ReadMeanValue(uint8_t channel, uint8_t nSamples)
{
	uint32_t Sum=0;
	// Reading values in a buffer
	for (uint8_t i=0; i < nSamples; i++)
	{
		Sum += ADC_ReadValue(channel);
	}
	
	return (uint16_t)(Sum / nSamples);
}

/**
 * @brief Read Reference Voltage (kind of offset measurement) 
 * 
 * This function returns ADC reference voltage (in mV), by reading and 
 * Comparing a reference voltage on channel. AVCC could be 5.2V, this
 * function would return 5200. 
 *
 * The ADC ref voltage, VCC or VREF INT are not so accurate indeed, if 
   the application require more accuracy, it's a good thing to 
 * measure the real value. (for Atmega8, VREF int goes from 2.4 to 2.6V!) 
 *
 * !! The voltage on selected channel must be very accurate 
 * 	(generated from a voltage reference for example) !!
 *
 * 	if a Voltage reference of 2.500 in connected to ADC CH6 
 * 		- ADC_ReadVref(6,2500) return the true value of ADC REF !
 *
 *
 * 
 * @param  channel Channel connected with a reference volltage 
 * @param  refVoltagemV Voltage of reference (mV)
 * @return uint16_t Value of ADC REF (in mV) 
 */
uint16_t ADC_ReadADCVref(uint8_t channel, uint16_t refVoltagemV)
{
	// returns Vref value in mV
	uint64_t vRef = 1024;
	vRef *= refVoltagemV;
	vRef /= ADC_ReadMeanValue(channel, 40);

	return (uint16_t)vRef;
}