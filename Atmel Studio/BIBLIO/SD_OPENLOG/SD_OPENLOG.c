/*
 * SD_ONPENLOG.c
 *
 * Created: 12.02.2024 10:07:12
 *  Author: adria
 */ 

#include "SD_OPENLOG.h"

/* State machine to know the actual state of the OPENLOG */
typedef enum
{
	UNKNOWN,
	WAIT_CMD,
	IS_WRITING,
	ERROR
}E_STATE_OPENLOG;

E_STATE_OPENLOG stateOpenlog = UNKNOWN;

/* Initialization of the Openlog
*
*	-> The Openlog is adressed to verify state and put in command mode
*
*/

uint8_t Openlog_Init()
{
	PORTB = 0x01;
	_delay_ms(50);
	uint8_t error = 0x01; 
	
	if (stateOpenlog == UNKNOWN)
	{	
		error = Openlog_CheckIfCmdMode();

		/* IF still in UNKNOWN STATE, can be in "Append mode", so 
		   to put Openlog in CMD mode */
		if (stateOpenlog == UNKNOWN)
		{
			error |= Openlog_SetCmdMode();
			
			if (error != 0)
			{
				stateOpenlog = ERROR;
			}
		}
	}
	return error;
}

/*
*	This function appends a string "str" the file called "filename"
* 
*	To avoid any errors, filename should follow 8.3 calling
*	file's norm. (example : test.txt)
*
*	The string should not be longer than TX_BUFFER
*
*/
uint8_t Openlog_AppendStringToFile(char * str, char * filename)
{
	uint8_t error = 0x00; 
	uint8_t nReceived = 0x00;
	
	char buf[UART_TX_BUFFER_SIZE];
	char readBuf[UART_RX_BUFFER_SIZE];
	
	if (stateOpenlog != UNKNOWN)
	{
		// Ensures that openlog is ready to receive a command
		if ((stateOpenlog == WAIT_CMD) || (stateOpenlog == ERROR))
		{
			error = Openlog_CheckIfCmdMode();
		}
		else
		{
			error = Openlog_SetCmdMode();
		}
	
		if ((stateOpenlog == WAIT_CMD) && (error == 0x00))
		{
			// succeed to put openlog in good mode 
			if (strlen(str) < UART_TX_BUFFER_SIZE)
			{
				// TODO - check filename format
				sprintf(buf,"append %s\r",filename);
			
				Uart_PutString(buf);
				_delay_ms(20);
				
				Uart_PutString(str);
				_delay_ms(20);
			
				nReceived = Uart_RxCountDataWaiting();
				
				if(nReceived > 0)
				{
					Uart_ReadBuffer(readBuf);
					
					for (uint8_t i = 0; i < nReceived; i++)
					{
						if (readBuf[i] == '<')
						{
							stateOpenlog = IS_WRITING;
						}
					}
					
					PORTB = 0x01;
					_delay_ms(50);
				
					if (stateOpenlog != IS_WRITING)
					{
						error |= 0x01;
						stateOpenlog = ERROR; 
					}
				}
			
				if (stateOpenlog == IS_WRITING)
				{
					
				}
				
				// Put in command mode after having send the 
				error = Openlog_SetCmdMode();
			
			}	
			else
			{
				// String to save is too long
				error |= 0x02;
			}
		}
		else
		{
			error |= 0x01;
		}
	}
	else
	{
		error = 0x04;
	}
	
	return error;
}


/*	YAT
	SENT VIA PC <- (10:05:31.530) (COM3) (<<) <CR> (1)
	OPENLOG		-> (10:05:31.575) (COM3) (>>) <CR><LF> (2)
	OPENLOG		-> (10:05:31.575) (COM3) (>>) > (1)
*/ 


uint8_t Openlog_SetCmdMode()
{
	uint8_t error = 0x01; 
	uint8_t tryCounter = 0x00;
	uint8_t nReceived = 0x00;
	
	char uartBuffer[UART_RX_BUFFER_SIZE];
	
	if (stateOpenlog != WAIT_CMD)
	{
		do
		{
			/*0x01A = CTRL + z*/
			UART_PutChar(0x1a);
			_delay_ms(10);
			UART_PutChar(0x1a);
			_delay_ms(10);
			UART_PutChar(0x1a);
			_delay_ms(30);
			
			nReceived = Uart_RxCountDataWaiting();
			
			if (nReceived > 0)
			{
				/* If received something within */
				Uart_ReadBuffer(uartBuffer);
				for (uint8_t i = 0; i < nReceived ; i++)
				{
					// TODO - check if '!' received (cmd not recognized)
					
					if (uartBuffer[i] == '>')
					{
						stateOpenlog = WAIT_CMD;
						error = 0x00;
						break;
					}
				}				
			}
			else
			{
				tryCounter ++;
			}
		}while((tryCounter < 3) && (stateOpenlog != WAIT_CMD));
		
		/* check if state has changed */
		if (error != 0)
		{
			stateOpenlog = ERROR; 
		}
	}
	else
	{
		/* Already in CMD state */
		error = 0x00; 
	}
	return error;	
}


uint8_t Openlog_CheckIfCmdMode()
{
	uint8_t error = 0x01; 
	uint8_t nReceived = 0x00;
	uint8_t tryCounter = 0x00; 
	char uartBuffer[UART_TX_BUFFER_SIZE];	
	
	do
	{
		UART_PutChar('\r');
		_delay_ms(20);			/* Should be sufficient */
		
		nReceived = Uart_RxCountDataWaiting();
		if (nReceived > 0)
		{
			/* If received something within */
			Uart_ReadBuffer(uartBuffer);
			for (uint8_t i = 0; i < nReceived ; i++)
			{
				if (uartBuffer[i] == '>')
				{
					stateOpenlog = WAIT_CMD;
					error = 0x00;
					break;
				}
			}
		}
		else
		{
			tryCounter++;
		}
	}while((tryCounter < 3) && (stateOpenlog != WAIT_CMD));
	
	if (error != 0)
	{
		stateOpenlog = ERROR;
	}
	
	return error;
}