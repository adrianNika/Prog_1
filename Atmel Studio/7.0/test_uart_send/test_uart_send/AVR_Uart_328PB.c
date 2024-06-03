/*********************************************************************************
*	@file		AVR_Uart.c
*	@brief		Uart library (ELO)
*
*	This library permits to use the UART peripheral of the AVR MCU we uses here.
*	It contains also the function to handle ELO protocol 
*
*	User must change the configuration in AVR_Uart.h in order to work in 
*	desired mode.
*
*	-File:		AVR_Uart.c
*	-Compiler:	AVR-GCC
*	-MCU:		Atmega8 & ATMega48-88-168-328

*	@Author		David Perrin
*	@version	1.0 
* 	@date		5/16/2022
*********************************************************************************/

#include "AVR_Uart_328PB.h"

// Global variables - Buffers 
// Buffer size can be changed in header file 
// Do not exceed 254 in order to use circular buffer
static char RX_Buffer[UART_RX_BUFFER_SIZE];
static char TX_Buffer[UART_TX_BUFFER_SIZE];

// Global Variables - Head's & Tails (circular buffer pointer) 
// See explanation under receive & send ISR
static volatile uint8_t RX_Head;	
static volatile uint8_t RX_Tail;
static volatile uint8_t TX_Head;
static volatile uint8_t	TX_Tail;

// Global variable -> ErrorUart 
static volatile uint8_t ErrorUart;

// Global Variable -> EloEndCharDetected
static volatile uint8_t EloEndCharDetected;

// Error types for ELO Protocol
char * ErrorsNoToken[5] =	{"ERROR : NO START FOUND*",
							 "ERROR : NO ID FOUND*",
							 "ERROR : NO DATA LENGTH FOUND*",
							 "ERROR : NO DATA FOUND*",
							 "ERROR : NO SECURTIY FOUND"};
	
// Protocol Error Messages 
char * ErrorEloProtocol[4] ={	"ERROR : BAD ID FORMAT (6 CHARS)*",
								"ERROR : BAD DATALEN FORMAT (3 CHARS)*",
								"ERROR : BAD DATA SIZE VS DATALEN*",
								"ERROR : BAD SECURITY FORMAT*"};
	
/**
 * @brief Initialization of UART 
 * 
 * Initialize UART configuration within configuration in AVR_Uart.h 
 * 
 * Baudrate & mode has to be configured in order to use this library correcly
 *
 * Resets pointers of circular buffer
 *	
 * @param void
 * @return void
 */
void UART_Init(void)
{
	// UCSRA , USCRB & UCSRC defined values in .h
	USART_CR_A = UCSRA_MODE_8N1;
	USART_CR_B = UCSRB_MODE_8N1;
	USART_CR_C = UCSRC_MODE_8N1;
	
	// Baudrate set-up (see header file) 
	BIT_RATE_REGISTER_H = (uint8_t)(VAL_UBRR >> 8);
	BIT_RATE_REGISTER_L = (uint8_t)(VAL_UBRR);
	
	// Reset position of Tails & Heads (circular buffers) 
	RX_Tail = 0;
	RX_Head = 0;
	TX_Tail = 0;
	TX_Head = 0;
}

/**
 * @brief Trigger an interrupt when UDR is empty (TX)
 * 
 * An UDRE interrupt occures if UDR register is empty (occures after data 
 * shifting in shift register). 
 *
 * When interrupt occures, UDR is ready to receive another data to be sent
 * A new data is placed in UDR & Tail's pointer of circular buffer is 
 * updated, until every data of buffer is sent out. 
 *
 * Circular buffer modification : +1 TX_Tail (consumming circular TX buffer) 
 *
 *
*/

ISR(USART0_UDRE_vect)
{
	uint8_t TmpTXTail;	// Temporary value of tail (for computation) 
	
	// if not all data transmitted 
	if(TX_Tail != TX_Head)	
	{
		TmpTXTail = TX_Tail + 1;			// Increase buffer index (new value to send)
		TmpTXTail %= UART_TX_BUFFER_SIZE;	// cycling buffer position if max reached
		TX_Tail = TmpTXTail;				// Saving New Tail position
		
		UDR_REG = TX_Buffer[TX_Tail];		// Put value in UDR for sending 
	}
	else
	{
		// Disable interrupt when all data sent out 
		USART_CR_B &= ~(1 << UDR_EMPTY_INT_EN);
	}
}

/**
 * @brief Trigger an interrupt when data received in UDR (RX)
 * 
 * When interrupt occures, the new data has to be stored in circular buffer.
 * Head pointer increases, only if buffer is not full.
 *
*/
ISR(USART0_RX_vect)
{
	char data;			// Temporary (to empty UDR)
	uint8_t tmpRXHead;	// Temporary (for computations) 
	
	data = UDR_REG;		// empty UDR in temporary variable 
						// this permits to receive another data (avoid loss)
	
	tmpRXHead = RX_Head + 1;			// Increase position of tmp head 
	tmpRXHead %= UART_RX_BUFFER_SIZE;	// cycling buffer position if max reached
	
	// Is RX_Buffer full ? 
	if (tmpRXHead == RX_Tail)
	{
		// --> ERROR : RX Buffer is full 
		ErrorUart |= UART_ERROR_RX_BUFFER_FULL; // Buffer is full, handle exeption ! 
	}
	else
	{
		RX_Head = tmpRXHead ;			// Save new pointer position
		RX_Buffer[tmpRXHead] = data;	// Save data in buffer
		
		// ELO Protocole handling
		if (data == '*')
		{
			Uart_SetELOEndCharDetected(0x01);
		}
	}
}


/**
 * @brief Put a character in buffer in order to be sent out 
 *
 * This functions put a char in buffer, only if TX buffer is not full 
 * (this buffer can be full when a lot of data has to be sent out rapidly) 
 *
 * If TX buffer is not full, place "data" in buffer TX_Head +1 and activate
 * "UDRE" interrupts (data to be sent). Otherwise, an error raises (ErrorUart)
 *
 * modify		 : TX_Head (adding data in buffer), ErrorUart, UDRIE (REG)
 *
 * @param  data char to be sent 
 * @return void nothing
 */
void UART_PutChar(char data)
{
	uint8_t tmpTxHead;		// Temporary buffer pointer (for computations)
	
	tmpTxHead = TX_Head +1;				// Increase position
	tmpTxHead %= UART_TX_BUFFER_SIZE;	// Cycle buffer if max reached
	
	// Is TX buffer Full ? 
	if (tmpTxHead == TX_Tail)
	{
		// Error --> TX buffer full (can no handle more data to send)
		ErrorUart |= UART_ERROR_TX_BUFFER_FULL;  
	}
	else
	{
		// Put data in circular buffer 
		TX_Buffer[tmpTxHead] = data;	
		
		// Save new head value
		TX_Head	= tmpTxHead;
		
		// Activate interrupt to transmit
		USART_CR_B |= (1 << UDR_EMPTY_INT_EN);
	}
}

/**
 * @brief
 *
 * modify		 : RX_Tail (consumming RX buffer), ErrorUart, 
 *
 * This functions reads a char in buffer, only if a new data has been 
 * received. Otherwise, an error is raised (ErrorUart) 
 *
 * @warning This Function Return 0 (Initialized value) 
 * if nothing received. 
 *
 * A Good practice is to check if a new data has been received before calling
 * this function and Check for errors after ! 
 * (We can receive "NULL" (ASCII = 0x00) data & not raise an error !) 
 *
 * @param  nothing
 * @return char received data
 */
char Uart_ReadChar(void)
{
	char data = 0;			// data to be read from UDR
	uint8_t tmpRxTail;		// Temporary RX tail (for computations) 
	
	// No new data received ? 
	if(RX_Tail == RX_Head)
	{
		// No data received, raises an error flag in ErrorUart
		ErrorUart |= UART_ERROR_NO_DATA_RECEIVED;
	}
	else
	{
		tmpRxTail = RX_Tail + 1;			// Increase pointer for new data
		tmpRxTail %= UART_RX_BUFFER_SIZE;	// Cycling buffer if max reached
		data = RX_Buffer[tmpRxTail];		// Read new value in buffer
		
		RX_Tail = tmpRxTail;				// Saves new position 
	}
	return data;		// return data (OR 0 if no new data) 
}

/**
 * @brief returns UartError
 *
 * This functions returns value of UartError (see Header for more information)
 *
 * @param  nothing
 * @return uint8_t value of UartError
 */
uint8_t Uart_GetError(void)
{
	return ErrorUart;
}

/**
 * @brief This functions Clears "UART_ERROR_NO_DATA_RECEIVED"
 * @param nothing
 * @return void nothing
 */
void Uart_ClearErrorNoDataRx(void)
{
	ErrorUart &= ~(UART_ERROR_NO_DATA_RECEIVED);
}

/**
 * @brief This functions Clears "UART_ERROR_RX_BUFFER_FULL"
 * @param  nothing
 * @return void nothing
 */
void Uart_ClearErrorRxFull(void)
{
	ErrorUart &= ~(UART_ERROR_NO_DATA_RECEIVED);
}

/**
 * @brief This functions Clears "UART_ERROR_TX_BUFFER_FULL"
 * @param  nothing
 * @return void nothing
 */
void Uart_ClearErrorTxFull(void)
{
	ErrorUart &= ~(UART_ERROR_TX_BUFFER_FULL);
}

/**
 * @brief Return number of data waiting to be read by user
 *
 * This functions returns number of data waiting in buffer, by computing with
 * Pointer positions. 
 *
 * @param  nothing
 * @return uint8_t number of data waiting
 */
uint8_t Uart_RxCountDataWaiting(void)
{
	uint8_t nbData =0; // Initialized = 0, if no data waiting
	
	// If Head has not been re-looped (MAX val -> 0)
	if (RX_Head > RX_Tail)
	{
		// number of data is only a difference of position
		nbData = RX_Head - RX_Tail;
	}
	
	// If Tail's loop cycle not occured (not full tho)
	if (RX_Head < RX_Tail)
	{
		// Compute number of data, according recycle of
		// head's pointer (according to UART_RX_BUFFER_SIZE)
		nbData = (UART_RX_BUFFER_SIZE - RX_Tail) + RX_Head;
	}
	
	// Return number of data waiting
	return nbData; 
}

/**
 * @brief return number of data waiting to be sent out by interruption
 *
 * This functions returns number of data waiting in TX buffer, by computing with
 * Pointer's positions.
 *
 * @param  
 * @return uint8_t number of data waiting to be sent
 */
uint8_t Uart_TxCountDataWaiting(void)
{
	uint8_t nbData = 0;	// Number of data waiting
	
	// If Head has not been re-looped (MAX val -> 0)
	if (TX_Head > TX_Tail)
	{
		nbData = TX_Head - TX_Tail;
	}
	
	// If Head has re-looped, compute according to buffer size
	if (TX_Head < TX_Tail)
	{
		nbData = (UART_TX_BUFFER_SIZE - TX_Tail) + TX_Head;
	}
	
	// If TX_Tail == TX_Head, no data is waiting, returns 0
	return nbData;
}

/**
 * @brief read RX buffer 
 *
 * This functions put in table pointed with "*strRead" the new data received
 * in RX buffer (Kind of a "Buffer Flush"). 
 *
 * @warning
 * - If no new data has bees received, nothing will be read
 * - strRead MUST point to a SAME SIZE or BIGGER char table ! Otherwise, 
 *   DATA COULD BE WRITTEN OUTSIDE OF TABLE (BIG ISSUE) !!!
 * .
 *
 * @param  strRead Pointer where to put data
 * @return uint8_t UartError Flag (0 if no error) 
 */
uint8_t Uart_ReadBuffer(char *strRead)
{
	// i = Increment var in for loop
	uint8_t i = 0;	
	
	// Counting number of data waiting to be read
	uint8_t dataAvaillable = Uart_RxCountDataWaiting();
	
	// Reading string only if data is availlable, otherwise,
	// an error raises (no data waiting error) 
	if (dataAvaillable > 0)
	{
		// data availlable, reading buffer char per char
		for (i = 0 ; i < dataAvaillable ; i++)
		{
			// Using function "Putchar" (Already treating head & Tail pointer) 
			strRead[i] = Uart_ReadChar();	
		}
	}
	else
	{
		// buffer too full for string
		ErrorUart |= UART_ERROR_NO_DATA_RECEIVED; 
	}
	
	// returning error, 0 if no error
	return ErrorUart;
}

/**
 * @brief Put a string into buffer (to be sent out) 
 *
 * This Function put the string pointed by *strSend in the circular buffer,
 * only if there is enough free space in buffer. 
 *
 * @warning
 * - If no space free in TX_Buffer, nothing will be put in buffer !
 * - strSend MUST be a pointer on a STRING, otherwise, no "end of string" could
 *   be detected("\0") , so nothing could be sent-out ! (Buffer != String) 
 * . 
 *
 * @param  strSend Pointer to string to send
 * @return uint8_t UartError Flag (0 if no error) 
 */
uint8_t Uart_PutString(char *strSend)
{
	// i = Increment var in for loop
	uint8_t i = 0;
	
	// Counting number of data waiting to be sent out
	uint8_t TxSpaceFree = UART_TX_BUFFER_SIZE - Uart_TxCountDataWaiting();
	
	// Getting string length
	uint8_t strLength = strlen((const char *) strSend);
	
	// Putting string only if enough space is availlable, otherwise,
	// an error raises 
	if (TxSpaceFree > strLength)
	{
		// data availlable, putting string in buffer char per char
		for (i = 0 ; i < strLength ; i++)
		{
			// Putting string char by char
			UART_PutChar(strSend[i]);
		}
	}
	else
	{
		// not enough space in buffer for string 
		ErrorUart |= UART_ERROR_TX_BUFFER_FULL;
	}
	
	// returning error, 0 if no error
	return ErrorUart;
}

/**
 * @brief put a buffer of char into TXbuffer to be sent out
 *
 * This Function put the content pointed by *strsend in the circular TX buffer,
 * only if there is free space (BufferLength <= FreeSpace). 
 *
 * @warning
 * - If no space free in TX_Buffer, nothing will be put in buffer !
 * - Bufferlength should never be higher than buffer to send, otherwise, 
 *   corrupted data could be sent out ! 	
 *
 * @param  strSend Pointer of buffer to send
 * @param  BufferLength Length of buffer to be sent out 
 * @return uint8_t UartError Flag (0 if no error) 
 */

uint8_t Uart_PutBuffer(char *strSend, uint8_t BufferLength)
{
		// i = Increment var in for loop
		uint8_t i = 0;
		
		// Counting number of data waiting to be sent out
		uint8_t TxSpaceFree = UART_TX_BUFFER_SIZE - Uart_TxCountDataWaiting();
		
		// Putting buffer only if enough space is availlable, otherwise,
		// an error raises
		if (TxSpaceFree >= BufferLength)
		{
			// data availlable, putting string in buffer char per char
			for (i = 0 ; i < BufferLength ; i++)
			{
				// Putting string char by char
				UART_PutChar(strSend[i]);
			}
		}
		else
		{
			// not enough space in buffer for string
			ErrorUart |= UART_ERROR_TX_BUFFER_FULL;
		}
		
		// returning error, 0 if no error
		return ErrorUart;	
}

/**
 * @brief Clear Tails & heads posititon (like USB fast format) 
 *
 * This function reset Tail & Head's positions to 0 
 *
 * This won't erase Buffers's Content, but new data will overwrite it.
 *
 * @param  nothing 
 * @return uint8_t UartError Flag (0 if no error)
 */
uint8_t Uart_ClearBuffer(void)
{
	// Resetting Tail & Head's positions 
	RX_Head = 0;
	RX_Tail = 0;
	TX_Head = 0;
	TX_Tail = 0; 
	
	// returning ErrorUart (no-cost operation) 
	return ErrorUart;
}

/**
 * @brief Sets End of CharDetected Flag (ELO PROTOCOL)
 * @param  value Value of flag to be set
 * @return void
 */
void Uart_SetELOEndCharDetected(uint8_t value)
{
	EloEndCharDetected = value;
}


/**
 * @brief Returns Value of EndOfCharDetected (ELO PROTOCOL)
 * @param  nothing 
 * @return uint8_t value of EloEndCharDetected
 */
uint8_t Uart_GetELOEndCharDetected(void)
{
	return (uint8_t)EloEndCharDetected;
}



/**
 * @brief Parse message in ELO PROTOCOL FORMAT
 *
 * Parse message of ELO PROTOCOL into 5 tokens. 
 * See xWiki for ELO PROTOCOL FORMAT, tokens are:
 *
 * EloTokens[0] = Start charracter ('$')
 * EloTokens[1] = Emitter ID (6 char)
 * EloTokens[2] = "data" length (3 char)
 * EloTokens[3] = "data" (1-999 chars)
 * EloTokens[4] = Security mode (1 char)
 *
 * All tokens are strings (char buffer terminated by '\0') 
 *
 * If a token is empty, error is raised in "noTokenFound"
 * If an error in ELO PROTOCOL format, error is raised in ErrorMsg
 *
 * @warning Buffer (Buf) has to be a string (terminated by '\0') 
 *
 * @param  Buf Uart received 
 * @param  EloTokens Tokens of ELOProtocol
 * @param  noTokenFound Pointer to userVarriable containing TokenError
 * @param  ErrorEloMsg Pointer to userVarriable containing protocolError
 * @return void
 */
void Uart_Elo_ParseMessage(char *Buf, char *EloTokens[5], uint8_t *noTokenFound, uint8_t *ErrorEloMsg)
{
	// Variable containing the length of data (ELO Protocol)
	uint16_t LengthData=0 ;
	
	// String Pointer
	char *Str;

	// Searching for start condition ('$')
	Str = strchr(Buf, '$');
				
	// Search in ELO String
		if (Str != NULL)
		{
			// Normally 5 token in a standard ELO comminucation
			for (uint8_t i = 0; i<5 ;i++)
			{
				// Separate by comma
				EloTokens[i] = strsep(&Str, ",");
						
				// Error if token not found
				if ((EloTokens[i] == NULL) || (EloTokens[i][0] == '\0'))
				{
					*noTokenFound |= (0x01 << i);
				}
			}
					
			// Only check if no error encountered
			if (*noTokenFound == 0)
			{
				// All Tokens have been found, we must
				// check them regarding length or value
				*ErrorEloMsg = 0; // assume no error
				
				// Check ID format	
				if (strlen(EloTokens[1]) != 6)
				{
					*ErrorEloMsg |= ELO_PROTOCOL_BAD_ID_FORMAT;
				}
				
				// Check DATALEN format	
				if (strlen(EloTokens[2]) != 3)
				{
					*ErrorEloMsg |= ELO_PROTOCOL_BAD_DATALEN_FORMAT;
				}
				else
				{
					// If format is OK, Compare Value with data size
					LengthData = atoi(EloTokens[2]);		// ATOI replacement (handmade)
					
					// Check if OK
					if (LengthData != strlen(EloTokens[3]))
					{
						*ErrorEloMsg |= ELO_PROTOCOL_BAD_DATA_SIZE;
					}
				}
						
				if (strlen(EloTokens[4]) != 1)
				{
					*ErrorEloMsg |= ELO_PROTOCOL_BAD_SECURITY_FORMAT;
				}				
			}
		}
		else
		{
			// No Start Token Found
			*noTokenFound |= ELO_PROTOCOL_NO_START_FOUND;
		}
}


/**
 * @brief shows Protocol Error (not in ELO FORMAT)
 *
 * This is a "debug" function, showing error while parsing 
 * ELO PROTOCOL message
 *
 * @param  Errtype var containing error
 * @return void
 */
void Uart_ShowProtocolError(uint8_t Errtype)
{
	char TXBuffer[70]; 
	
	for (uint8_t i = 0; i < 4; i++)
	{
		if ((Errtype & 0x01) == 0x01)
		{
			sprintf(TXBuffer,"%s", ErrorEloProtocol[i]);
			while((UART_TX_BUFFER_SIZE - Uart_TxCountDataWaiting()) < strlen(TXBuffer));
			Uart_PutBuffer(TXBuffer, strlen(TXBuffer));
			
		}
		Errtype /= 2;
	}
}

/**
 * @brief shows Token Error (not in ELO FORMAT)
 *
 * This is a "debug" function, showing Token error while parsing 
 * ELO PROTOCOL message. 
 *
 * @param  Errtype var containing error
 * @return void
 */
void Uart_ShowTokenError (uint8_t TokError)
{
	char TXBuffer[70];
	
	// Display Error if a token is missing
	for (uint8_t i = 0; i < 5 ; i++)
	{
		if ((TokError & 0x01) == 0x01)
		{
			sprintf(TXBuffer,"%s",ErrorsNoToken[i]);
			while((UART_TX_BUFFER_SIZE - Uart_TxCountDataWaiting()) < strlen(TXBuffer));
			Uart_PutBuffer(TXBuffer, strlen(TXBuffer));
			_delay_ms(50);
		}
		TokError /= 2;
	}
}

void Uart_select_peripherique(uint8_t select)
{
	if (select == 0)
	{
		_delay_ms(50);
		PORTE = 0x02;
		_delay_ms(50);
	}
	else if (select == 1)
	{
		_delay_ms(50);
		PORTE = 0x01;
		_delay_ms(50);
	}
	else
	{
		_delay_ms(50);
		PORTE = 0x03;
		_delay_ms(50);
	}
}