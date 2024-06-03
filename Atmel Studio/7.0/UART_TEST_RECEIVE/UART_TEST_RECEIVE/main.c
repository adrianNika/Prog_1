/*
 * UART_TEST_RECEIVE.c
 *
 * Created: 22/06/2023 10:21:08
 * Author : Fix-5
 */ 

#include "MCP_23017_I2C.h"

#define SLAVE_ADDR 0x20

U_TWI_MSG MCP_Swisspi;
U_InOut McpCtrl;

typedef enum
{
	OPERATION_PLUS,
	OPERATION_MOIN,
	OPERATION_FOIX,
	OPERATION_DIVISER
}E_SM_CALCULE;

E_SM_CALCULE choixOperation = OPERATION_PLUS;

int main(void)
{
	// EndOfELOCommand = 1 if end char found in Elo Protocol (AVR_Uart.c)
	volatile uint8_t EndOfELOCommand=0;
	
	// TXBuffer: Buffer to be sent back
	char TXBuffer[UART_TX_BUFFER_SIZE];
	char RXBuffer[UART_RX_BUFFER_SIZE];
	
	// Contains number of data received
	uint8_t CountDataReceived=0;
	
	// Contain error concerning tokens (missing token)
	uint8_t noTokenFound = 0;
	
	// Contains error concerning ELO Protocol
	uint8_t ELOProtocolError = 0;
	
	// Contains Tokens of ELO Protocol
	char *EloTokens[5];
	
	// After local parsing: Contains destination ID of Msg
	char *destID;

	char *calcule;
	uint16_t valuecal1 = 0x0000;
	uint16_t valuecal2 = 0x0000;
	uint16_t total = 0x0000;
	
	uint8_t flagCalcule = 0x0000;
	
	char UartMsg[100];
	
	sei();
	
	TWIMaster_Init();
	
	MCP_INIT(/*0x00*/);
	
	UART_Init();
	
	MCP_GPIOA(0x00);
	MCP_GPIOB(0x00);
	
	MCP_OUT(0);
	
	while (1)
	{
		// Check if end of char detected (see AVR_Uart.c)
		EndOfELOCommand = Uart_GetELOEndCharDetected();		// Caractre de fin dtect ?
		
		// If the end of char is detected ("*")
		if (EndOfELOCommand > 0)
		{
			// Clear Flag (Only process one message)
			Uart_SetELOEndCharDetected(0x00);
			
			// Get Number number of char received
			CountDataReceived = Uart_RxCountDataWaiting();
			
			// @todo UART_RX_CHECK check before reading buffer
			Uart_ReadBuffer(RXBuffer);
			
			// Reading for buffer, searching for end char
			// Transform ELO Message into a string
			for (uint8_t i =0; i<CountDataReceived;i++)
			{
				// Finding the end of ELO message
				if (RXBuffer[i] == '*')
				{
					// Replacing with \0
					RXBuffer[i] = '\0';
					break;
				}
			}
			
			// Assume no Message error before parsing
			noTokenFound=0;
			ELOProtocolError=0;
			
			// Decoding ELO protocol
			Uart_Elo_ParseMessage(RXBuffer,EloTokens,&noTokenFound,&ELOProtocolError);
			
			
			if((noTokenFound == 0) && (ELOProtocolError == 0))
			{
				// Getting Destination ID
				destID = strsep( &EloTokens[3] , "-");
				
				// If Message concerning ourself
				if (strcmp(destID,"SWPI20") == 0)
				{
					calcule = strchr(EloTokens[3], '+');
					if (calcule != NULL)
					{
						choixOperation = OPERATION_PLUS;
						calcule = strsep(&EloTokens[3], "+");
					}
					else
					{
						calcule = strchr(EloTokens[3], '-');
						if (calcule != NULL)
						{
							choixOperation = OPERATION_MOIN;
							calcule = strsep(&EloTokens[3], "-");
						}
						else
						{
							calcule = strchr(EloTokens[3], 'X');
							if (calcule != NULL)
							{
								choixOperation = OPERATION_FOIX;
								calcule = strsep(&EloTokens[3], "X");
							}
							else
							{
								calcule = strchr(EloTokens[3], '/');
								if (calcule != NULL)
								{
									choixOperation = OPERATION_DIVISER;
									calcule = strsep(&EloTokens[3], "/");
								}
							}
						}
					}
					
					valuecal1 = strtoul(calcule, NULL, 16);
					
					calcule = strsep(&EloTokens[3], "!");
					
					valuecal2 = strtoul(calcule, NULL, 16);
					
					flagCalcule = 0x01;
				}
			}
		}
		// Check for UART noise filling buffer
		if (Uart_RxCountDataWaiting() >= (UART_RX_BUFFER_SIZE -1))
		{
			// Clear Buffer, due to spam
			Uart_ClearBuffer();
		}
		
		if (flagCalcule != 0x00)
		{
			flagCalcule = 0x00;
			
			switch (choixOperation)
			{
				case OPERATION_PLUS:
					total = valuecal1 + valuecal2;
				break;
				
				case OPERATION_MOIN:
					total = valuecal1 - valuecal2;
				break;
				
				case OPERATION_FOIX:
					total = valuecal1 * valuecal2;
				break;
				
				case OPERATION_DIVISER:
					total = valuecal1 / valuecal2;
				break;
			}
			
			MCP_OUT(total);
			sprintf(UartMsg, "0x%04x", total);
			Uart_PutString(UartMsg);
		}
	}
}