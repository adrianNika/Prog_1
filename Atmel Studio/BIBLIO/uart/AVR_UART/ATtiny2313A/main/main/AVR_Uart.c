/*
 * AVR_Uart.c
 *
 * Auteur :		David Perrin
 * Date :		15.02.2021
 * Version :	V0
 *
 * Description : 
 *		
 *		This library suits for : 
 *			- ATMEGA8 et ATMEGA8L
 *			- ATMEGA48A/88A/168A/328A et PA 
 *			- ATTiny2313A/PA
*/
#define F_CPU 16000000UL		// Fréquence du CPU 
#include "AVR_Uart.h"

// Global variables - Buffers 
// Buffer size can be changed in header file 
// Do not exceed 254 
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


/******************************************************************************/
//	UART_Init - UART Initialization 
//	
//	Initialize UART configuration register with desired value, depending on 
//	wanted working mode. 
//	
//	Defines have to be modified in AVR_UART.h for another working mode !
//
//	Circular Buffer modification : Pointers set to 0 
//
/******************************************************************************/
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

/******************************************************************************/
//	ISR - UDRE Interrupt 
//
//	An UDRE interrupt occures if UDR register is empty (occures after data 
//	shifting in shift register). 
//
//	When interrupt occures, UDR is ready to receive another data to be sent
//	A new data is placed in UDR & Tail's pointer of circular buffer is 
//	updated, until every data of buffer is sent out. 
//
//	Circular buffer modification : +1 TX_Tail (consumming circular TX buffer)
//
/******************************************************************************/ 
ISR(USART_UDRE_vect)
{
	uint8_t TmpTXTail;	// Temporary value of tail (for computation) 
	
	// All data transmitted 
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

/******************************************************************************/
//	ISR - RX Interrupt
//	RX interrupt occures when a new data is received. 
//	
//	-->> Receive : nothing
//	<<-- Return	 : nothing
//	modify		 : RX_Head (adding data), ErrorUart
//
//	When interrupt occures, the new data has to be stored in circular buffer. 
//	Head pointer increases, only if buffer is not full. 
//
/******************************************************************************/
ISR(USART_RX_vect)
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
	}
}

/******************************************************************************/
//	Function - Uart_Putchar
//
//	-->> Receive : data (to be sent out from buffer in interrupt) 
//	<<-- Return	 : nothing
//	modify		 : TX_Head (adding data in buffer), ErrorUart, UDRIE (REG)
// 
//	This functions put a char in buffer, only if TX buffer is not full 
//	(this buffer can be full when a lot of data has to be sent out rapidly) 
//
//	If TX buffer is not full, place "data" in buffer TX_Head +1 and activate
//	"UDRE" interrupts (data to be sent). Otherwise, an error raises (ErrorUart)
//
/******************************************************************************/
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

/******************************************************************************/
//	Function - Uart_ReadChar
//
//	-->> Receive : Nothing
//	<<-- Return	 : data
//	modify		 : RX_Tail (consumming RX buffer), ErrorUart, 
//
//	This functions reads a char in buffer, only if a new data has been 
//	received. Otherwise, an error is raised (ErrorUart) 
//
//	!!! WARNING !!! This Function Return 0 (Initialized value) 
//	if nothing received. 
//
//	A Good practice is to check if a new data has been received before calling
//	this function and Check for errors after ! 
//	(We can receive "NULL" (ASCII = 0x00) data & not raise an error !) 
//
/******************************************************************************/
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

/******************************************************************************/
//	Function - Uart_GetError
//
//	-->> Receive : Nothing
//	<<-- Return	 : ErrorUart
//	modify		 : Nothing
//
//	This functions returns value of UartError (see Header for more information)
//
/******************************************************************************/
uint8_t Uart_GetError(void)
{
	return ErrorUart;
}

/******************************************************************************/
//	Function - Uart_ClearErrorNoDataRx
//
//	-->> Receive : Nothing
//	<<-- Return	 : Nothing
//	modify		 : Error Uart
//
//	This functions Clears "UART_ERROR_NO_DATA_RECEIVED"
//
/******************************************************************************/
void Uart_ClearErrorNoDataRx(void)
{
	ErrorUart &= ~(UART_ERROR_NO_DATA_RECEIVED);
}

/******************************************************************************/
//	Function - Uart_ClearErrorRxFull
//
//	-->> Receive : Nothing
//	<<-- Return	 : Nothing
//	modify		 : ErrorUart
//
//	This functions Clears "UART_ERROR_RX_BUFFER_FULL"
//
/******************************************************************************/
void Uart_ClearErrorRxFull(void)
{
	ErrorUart &= ~(UART_ERROR_NO_DATA_RECEIVED);
}

/******************************************************************************/
//	Function - Uart_ClearErrorTxFull
//
//	-->> Receive : Nothing
//	<<-- Return	 : Nothing
//	modify		 : ErrorUart
//
//	This functions Clears "UART_ERROR_TX_BUFFER_FULL"
//
/******************************************************************************/
void Uart_ClearErrorTxFull(void)
{
	ErrorUart &= ~(UART_ERROR_TX_BUFFER_FULL);
}

/******************************************************************************/
//	Function - Uart_RxCountDataWaiting
//
//	-->> Receive : Nothing
//	<<-- Return	 : nbData (Number of data waiting in RX buffer)
//	modify		 : Nothing
//
//	This functions returns number of data waiting in buffer, by computing with 
//	Pointer positions. 
//
/******************************************************************************/
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

/******************************************************************************/
//	Function - Uart_TxCountDataWaiting
//
//	-->> Receive : Nothing
//	<<-- Return	 : nbData (Number of data waiting to be sent in TX buffer)
//	modify		 : nothing
//
//	This functions returns number of data waiting in TX buffer, by computing with
//	Pointer's positions.
//
/******************************************************************************/
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

/******************************************************************************/
//	Function - Uart_ReadBuffer
//
//	-->> Receive : *strRead (Pointer on char table)
//	<<-- Return	 : ErrorUart 
//	modify		 : ErrorUart
//
//	This functions put in table pointed with "*strRead" the new data received
//	in RX buffer (Kind of a "Buffer Flush"). 
//
//	!!! WARNING !!!
//	- If no new data has bees received, nothing will be read
//	- strRead MUST point to a SAME SIZE or BIGGER char table ! Otherwise, 
//	  DATA COULD BE WRITTEN OUTSIDE OF TABLE (BIG ISSUE) !!!
/******************************************************************************/
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

/******************************************************************************/
//	Function - Uart_PutString
//
//	-->> Receive : *strSend (Pointer on char table)
//	<<-- Return	 : ErrorUart
//	modify		 : ErrorUart
//
//	This Function put the string pointed by *strSend in the circular buffer,
//	only if there is enough free space in buffer. 
//
//	!!! WARNING !!!
//	- If no space free in TX_Buffer, nothing will be put in buffer !
//	- strSend MUST be a pointer on a STRING, otherwise, no "end of string" could
//	  be detected("\0") , so nothing could be sent-out ! (Buffer != String) 
//	
/******************************************************************************/

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

/******************************************************************************/
//	Function - Uart_PutString
//
//	-->> Receive : *strSend (Pointer on char table), BufferLength
//	<<-- Return	 : ErrorUart
//	modify		 : ErrorUart
//
//	This Function put the content pointed by *strsend in the circular TX buffer, 
//	only if there is free space (BufferLength <= FreeSpace). 
//
//	!!! WARNING !!!
//	- If no space free in TX_Buffer, nothing will be put in buffer !
//	- Bufferlength should never be higher than buffer to send, otherwise, 
//	  corrupted data could be sent out ! 	
/******************************************************************************/

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

/******************************************************************************/
//	Function - Uart_ClearBuffer
//
//	-->> Receive : nothing
//	<<-- Return	 : ErrorUart
//	modify		 : RX_Head, RX_Tail, TX_Head, TX_Tail
//
//	This function reset Tail & Head's positions to 0 
//	
//	This won't erase Buffers's Content, but new data will overwrite it. 
//	
/******************************************************************************/

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
