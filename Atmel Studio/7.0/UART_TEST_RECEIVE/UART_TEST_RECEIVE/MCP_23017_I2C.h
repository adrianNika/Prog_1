/*
 * MCP_23017_I2C.h
 *
 * Created: 27/03/2023 11:14:08
 *  Author: Fix-5
 */ 


#ifndef MCP_23017_I2C_H_
#define MCP_23017_I2C_H_

#include "global.h"

typedef union
{
	struct
	{
		uint8_t SetRegsMcp:8;
	}S_McpRegs;
	
	uint8_t byte;
}U_InOut;


typedef union
{
	struct
	{
		uint16_t in_1:1;
		uint16_t in_2:1;
		uint16_t in_4:1;
		uint16_t in_8:1;
		uint16_t in_16:1;
		uint16_t in_32:1;
		uint16_t in_64:1;
		uint16_t in_128:1;
		
	}S_McpInput;
	
	uint16_t byte;
}U_InPut;
/****************************************************************************
************************************FUNCTION*********************************
****************************************************************************/

void MCP_INIT(/*uint8_t valueIOCON*/);

void MCP_GPIOA(uint8_t IOA);
void MCP_GPIOB(uint8_t IOB);

void MCP_OUT(uint16_t valueOut);
uint16_t MCP_IN(void);


#endif /* MCP_23017_I2C_H_ */