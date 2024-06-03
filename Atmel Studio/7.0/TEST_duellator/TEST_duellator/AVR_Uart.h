/*
 * AVR_Uart.h
 *
 * Auteur :		David Perrin
 * Date :		15.02.2021
 * Version :	V0
 *
 * Description : 
 *		Defines, Includes & Prototypes of AVR_Uart.h
 *
 *		Comment/Uncomment following desired working mode
 *		
 *		MCU compatibility : 
 *
 *			- ATMEGA48A/88A/168A/328A & PA 
 *			- ATTiny2313A/PA
*/ 

// Avoid "double - include issues"
#ifndef AVR_UART_H		
#define AVR_UART_H

#include "global.h"

// Utilities includes
#include <stdint-gcc.h>		// TO DO : Verify AVR4 & WinAVR compatibility

#include <avr/io.h>			// Device should be selected to use this.
							// Automaticaly done by environment (ATmelStudio 7)
							// Otherwise, you have to define the good one manually
							// by writing #define >>>AVR NAME OF MCU<<< (see io.h)
							// Define must be placed before include

#include <avr/interrupt.h>	// Using Interrupt ISR recognition and "SEI/CLI"

#include <string.h>			// Used for "STRLEN" in function "Uart_PutString"
							// (Can be done manually by tracking caracter "\0")
							
#include <stdio.h>			// Using "sprintf"

#include <util/delay.h>

#include "stdlib.h"

// User Defines
// Buffer size for RX & TX in 8byte unsigned (uint8_t) (circular buffers)
#define UART_TX_BUFFER_SIZE 70		
#define UART_RX_BUFFER_SIZE 70

// Baudrate definitions 
// 
// Baudrate, F_CPU & U2x determines UBRRH/UBRRL (16 bit value) values to use. 
// U2X is the second bit of UCSRA
//
// IF U2xn = 0 --> UBRR = (F_CPU / (8 * Baudrate)) -1
// IF U2xn = 1 --> UBRR = (F_CPU / (16 * Baudrate)) -1
// 
// This table shows UBRR value for common Baudrates @F_CPU = 1MHz, 8MHz, 16MHz
// (see datasheet for more info) : 
// 
//			 _________1MHz______||______8MHz________||______16MHz		||
//  bd rate	|| U2Xn=0	|U2xN=1	|| U2Xn=0	|U2xN=1	|| U2X = 0	|U2xN=1	||
//	  2400	||	  25	|	51	||	 207	|  416	||	 416	|  832	||
//	  4800	||	  12	|	25	||	 103	|  207	||	 207	|  416	||
//	  9600	||	   6*	|	12	||	  51	|  103	||	 103	|  207 	||		
//  14'400	||	   3*	|	8*	||	  34*	|   68*	||	  68*	|  138	||	
//  19'200	||	   2*	|	6*	||	  25	|  	51	||	  51	|  103	||
//  28'800	||	   1*	|	3*	||	  16*	|   34*	||	  34*	|	68*	|| !!
//  38'400	||	   1*	|	2*	||	  12	|   25	||	  25	|	51	||
//  57'600	||	   0*	|	1*	||	   8*	|   16*	||	  16*	|	34*	|| !! 
//	76'800	||	   -	|	1*	||	   6*	|   12	||	  12	|	25	||
// 115'200	||	   -	|	0*	||	   3*	|    8*	||	   8*	|	16*	|| !!
// 230'400	||	   -	|	-	||	   1*	|	 3*	||	   3*	|	 8*	|| !!
// 250'000	||	   -	|	-	||	   1	|	 3	||	   3	|	 7	||
// 500'000	||	   -	|	-	||	   0	|	 1 	||	   1 	|	 3	||
// 1M		||	   -	|	-	||	   -	|	 0	||	   0	|	 1	||		
//
// --> * = error > 0,5% - avoid using those baudrates with your Xtal
// --> !! = do not use (for perfect baudrate match, use a 14.7456MHz XTAL). 

#define	VAL_UBRR	103		// [dec] 9'600 bauds with 16 MHz F_CPU and U2xN = 1


// ---- REGISTER NAME DEFINITION ----
// register names are re-defined for portability within different MCU
// Defines for names of registers (UCRSA, UCRSB, UCRSC) 
// Comment/uncomment following MCU you use ! 

/*
// -- ATMEGA8 et PA Definitions ----------------------------------------------------------
// - UCRSA - UART Configuration register A
//
// UCSR0A ->	| RXC0 | TXC0 | UDRE0 | FE0 | DOR0 | UPE0 | U2X0 | MPCM0
// read/write	   R	 R/(W)	  R		 R		R	   R    R/W     R/W

#define USART_CR_A			UCSRA
#define RX_CMPLT_FLAG		RXC		// = 1 when data received in UDR
#define TX_CMPLT_FLAG		TXC		// = 1 when data sent from Shift register
#define UDR_EMPTY_FLAG		UDRE	// = 1 when UDR is ready to receive new data
#define FRAME_ERROR_FLAG	FE		// = 1 when a frame error occures DS p.201
#define DATA_OVERRUN_FLAG	DOR		// = 1 when start detected and UDR not ready
#define PARITY_ERROR_FLAG	PE		// = 1 when polarity error (odd/even)
#define DOUBLE_SPEED_U2X	U2X		// = 1 to double frequency of USART
#define MULTI_MCU_EN		MPCM	// = 1 to activate Multi MCU mode

// -- ATMEGA48A/88A/168A/328A et PA Definitions ---------------------------------------------
// - UCRSB
//
// UCSR0B -> | RXCIE0 | TXCIE0 | UDREIE0 | RXEN0 | TXEN0 | UCSZ02 | RXB80 | TXB80

#define USART_CR_B			UCSRB	// = Control register for UART, REGISTER B
#define RXC_INT_EN			RXCIE	// = 1 to activate RX interrupts
#define TXC_INT_EN			TXCIE	// = 1 to activate TX interrupts
#define UDR_EMPTY_INT_EN	UDRIE	// = 1 to activate UDR empty Interrupts
#define RX_ENABLE			RXEN	// = 1 to activate RX interface
#define TX_ENABLE			TXEN	// = 1 to activate TX interface
#define CHAR_SIZE_2			UCSZ2	// = Char size bit 2 (see table on datasheet)
#define RX_BIT_8			RXB8	// = 9th bit when 9 bit mode
#define TX_BIT_8			TXB8	// = 9th bit when 9 bit mode

// UCSR0C -> | UMSEL01 | UMSEL00 | UPM01 | UPM00 | USBS0 | UCSZ01 | UCSZ00 | UCPOL0

#define USART_CR_C			UCSRC
#define USART_MODE_SEL1		URSEL	// mode select bit 1
#define USART_MODE_SEL0		UMSEL	// mode select bit 0 --> DS p.202
#define USART_PARITY_MODE1	UPM1	// parity bit 1
#define USART_PARITY_MODE0	UPM0	// parity bit 0 --> DS p. 203
#define USART_STOP_B_SEL	USBS	// = 0 for 1 stop bit, =1 for 2 stop bits
#define CHAR_SIZE1			UCSZ1	// Char size bit 1
#define CHAR_SIZE0			UCSZ0	// CHAR size bit 0 --> See DS p.203
#define CLOCK_POL			UCPOL	// clock polarity (Synchronous mode only)

// - UBBR
#define BIT_RATE_REGISTER_H	UBRRH	//	8 MSB of 16 bit's UBRR0
#define BIT_RATE_REGISTER_L	UBRRL	//	8 LSB of 16 bit's UBRR0

// - UDR
#define UDR_REG	UDR	// UDR - data register

*/

// -- ATMEGA48A/88A/168A/328A et PA Definitions ----------------------------------------------
// - UCRSA - UART Configuration register A
//	
// UCSR0A ->	| RXC0 | TXC0 | UDRE0 | FE0 | DOR0 | UPE0 | U2X0 | MPCM0
// read/write	   R	 R/(W)	  R		 R		R	   R    R/W     R/W

#define USART_CR_A			UCSR0A
#define RX_CMPLT_FLAG		RXC0	// = 1 when data received in UDR
#define TX_CMPLT_FLAG		TXC0	// = 1 when data sent from Shift register
#define UDR_EMPTY_FLAG		UDRE0	// = 1 when UDR is ready to receive new data
#define FRAME_ERROR_FLAG	FE0		// = 1 when a frame error occures DS p.201	
#define DATA_OVERRUN_FLAG	DOR0	// = 1 when start detected and UDR not ready
#define PARITY_ERROR_FLAG	UPE0	// = 1 when polarity error (odd/even)
#define DOUBLE_SPEED_U2X	U2X0	// = 1 to double frequency of USART
#define MULTI_MCU_EN		MPCM0	// = 1 to activate Multi MCU mode 

// -- ATMEGA48A/88A/168A/328A et PA Definitions ---------------------------------------------
// - UCRSB
//
// UCSR0B -> | RXCIE0 | TXCIE0 | UDREIE0 | RXEN0 | TXEN0 | UCSZ02 | RXB80 | TXB80

#define USART_CR_B			UCSR0B	// = Control register for UART, REGISTER B
#define RXC_INT_EN			RXCIE0	// = 1 to activate RX interrupts
#define TXC_INT_EN			TXCIE0	// = 1 to activate TX interrupts
#define UDR_EMPTY_INT_EN	UDRIE0	// = 1 to activate UDR empty Interrupts
#define RX_ENABLE			RXEN0	// = 1 to activate RX interface
#define TX_ENABLE			TXEN0	// = 1 to activate TX interface
#define CHAR_SIZE_2			UCSZ02	// = Char size bit 2 (see table on datasheet)
#define RX_BIT_8			RXB80	// = 9th bit when 9 bit mode
#define TX_BIT_8			TXB80	// = 9th bit when 9 bit mode 

// UCSR0C -> | UMSEL01 | UMSEL00 | UPM01 | UPM00 | USBS0 | UCSZ01 | UCSZ00 | UCPOL0

#define USART_CR_C			UCSR0C
#define USART_MODE_SEL1		UMSEL01	// mode select bit 1
#define USART_MODE_SEL0		UMSEL00	// mode select bit 0 --> DS p.202
#define USART_PARITY_MODE1	UPM01	// parity bit 1
#define USART_PARITY_MODE0	UPM00	// parity bit 0 --> DS p. 203
#define USART_STOP_B_SEL		USBS0	// = 0 for 1 stop bit, =1 for 2 stop bits
#define CHAR_SIZE1			UCSZ01	// Char size bit 1
#define CHAR_SIZE0			UCSZ00	// CHAR size bit 0 --> See DS p.203
#define CLOCK_POL				UCPOL0	// clock polarity (Synchronous mode only)

// - UBBR
#define BIT_RATE_REGISTER_H	UBRR0H	//	8 MSB of 16 bit's UBRR0
#define BIT_RATE_REGISTER_L	UBRR0L	//	8 LSB of 16 bit's UBRR0

// - UDR
#define UDR_REG	UDR0	// UDR - data register

// UNCOMMENT BELOW TO USE ATTINY 2313 -----------------------------------------------------
// -- ATTINY2313/A Definitions --  
/*
// - UCRSA
#define USART_CR_A			UCSRA
#define RX_CMPLT_FLAG		RXC		
#define TX_CMPLT_FLAG		TXC
#define UDR_EMPTY_FLAG		UDRE
#define FRAME_ERROR_FLAG	FE
#define DATA_OVERRUN_FLAG	DOR
#define PARITY_ERROR_FLAG	UPE
#define DOUBLE_SPEED_U2X	U2X
#define MULTI_MCU_EN		MPCM

// - UCRSB
#define USART_CR_B			UCSR0B
#define RXC_INT_EN			RXCIE
#define TXC_INT_EN			TXCIE
#define UDR_EMPTY_INT_EN	UDRIE
#define RX_ENABLE			RXEN
#define TX_ENABLE			TXEN
#define CHAR_SIZE_2			UCSZ2
#define RX_BIT_8			RXB8
#define TX_BIT_8			TXB8

// - UCRSC
#define USART_CR_C			UCSRC
#define USART_MODE_SEL1		UMSEL1
#define USART_MODE_SEL0		UMSEL0
#define USART_PARITY_MODE1	UPM1
#define USART_PARITY_MODE0	UPM0
#define USART_STOP_B_SEL	USBS
#define CHAR_SIZE1			UCSZ1
#define CHAR_SIZE0			UCSZ0
#define CLOCK_POL			UCPOL

// - UBRR
#define BIT_RATE_REGISTER_H	UBRRH
#define BIT_RATE_REGISTER_L	UBRRL

// - UDR
#define UDR_REG	UDR0	// UDR - data register
*/ 


// UART MODES
// Definitions on UCS registers A,B & C. 
// Modify those definition for different working mode 
#define UCSRA_MODE_8N1		0b00000010	// U2X = 1 !
#define UCSRB_MODE_8N1		0b10111000	// RXCIE0, UDRIE0, RXEN, TXEN	= 1
#define UCSRC_MODE_8N1		0b00000110	// UCSZ01, UCSZ00				= 1

// UART ERROR
// Error Flags (1 bit = 1 error) 
#define UART_NO_ERROR					0x00
#define UART_ERROR_NO_DATA_RECEIVED		0x01
#define	UART_ERROR_RX_BUFFER_FULL		0x02
#define UART_ERROR_TX_BUFFER_FULL		0x04

// Protocol ELO Errors

#define ELO_PROTOCOL_NO_START_FOUND 0x01

#define ELO_PROTOCOL_BAD_ID_FORMAT 0x01
#define ELO_PROTOCOL_BAD_DATALEN_FORMAT 0x02
#define ELO_PROTOCOL_BAD_DATA_SIZE 0x04
#define ELO_PROTOCOL_BAD_SECURITY_FORMAT 0x08

// User Types


// User Prototypes
// See AVR_Uart.c for more information about those 
// functions
void UART_Init(void);
void UART_PutChar(char data);
char Uart_ReadChar(void);

uint8_t Uart_GetError(void);
void Uart_ClearErrorNoDataRx(void);
void Uart_ClearErrorRxFull(void);
void Uart_ClearErrorTxFull(void);

uint8_t Uart_RxCountDataWaiting(void);
uint8_t Uart_TxCountDataWaiting(void);

uint8_t Uart_ReadBuffer(char *strRead);
uint8_t Uart_PutString(char *strSend);
uint8_t Uart_PutBuffer(char *strSend, uint8_t BufferLength);

uint8_t Uart_ClearBuffer(void);

void Uart_SetELOEndCharDetected(uint8_t value);
uint8_t Uart_GetELOEndCharDetected(void);


void Uart_ShowTokenError (uint8_t TokError);

void Uart_ShowProtocolError(uint8_t Errtype);


void Uart_Elo_ParseMessage(char *Buf, char *EloTokens[5], uint8_t *noTokenFound, uint8_t *ErrorEloMsg);
#endif

