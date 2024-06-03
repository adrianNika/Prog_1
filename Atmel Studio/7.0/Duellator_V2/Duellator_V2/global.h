/*********************************************************************************
*	@file		global.h
*	@brief		Global include file
*
*	Include this to all header files. 
*	If a dependency is needed in all modules (.c), add it here as well. 
*
*	-File:		global.h
*	-Compiler:	AVR-GCC
*	-MCU:		ATmega8

*	@Author		David Perrin
*	@version	1 
* 	@date		5/13/2022
*********************************************************************************/

#ifndef GLOBAL_H_
#define GLOBAL_H_
	
	// Frequency of MCU
	#define	F_CPU	8000000UL

	// Using _delay_ms() functions
	#include <util/delay.h>
	
	// MCU register abreviations
	#include <avr/io.h>
	
	// Using standard types
	#include <stdint-gcc.h>
	
	#include "AVR_Uart.h"
	#include "TWI_Master.h"
	#include "LCD_I2C.h"
	#include <stdio.h>
	#include <avr/interrupt.h>
	#include <avr/eeprom.h>
	#include <avr/wdt.h>
	#include <string.h>
	
	#define SLAVEADDR_PCF8574A 0x38

#endif /* GLOBAL_H_ */