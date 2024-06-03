/*
 * MCP_23017_I2C.c
 *
 * Created: 27/03/2023 11:13:43
 *  Author: Fix-5
 */ 

#include "MCP_23017_I2C.h"

/*define debug_Mode*/
 //#define _LCD_DEBUG_MODE 1
/*define debug_Mode*/

#ifdef _LCD_DEBUG_MODE
	#include <stdio.h>
#endif

#define SLAVE_ADDR 0x20

#define IOCON  0x05


/*IOCON.BANK = 0 (default)*/
#define IODIRA		0x00
#define IODIRB		0x01
#define IPOLA		0x02
#define IPOLB		0x03
#define GPINTENA	0x04
#define GPINTENB	0x05
#define DEFVALA		0x06
#define DEFVALB		0x07
#define INTCONA		0x08
#define INTCONB		0x09
#define IOCONA		0x0A
#define IOCONB		0x0A
#define GPPUA		0x0C
#define GPPUB		0x0D
#define INTFA		0x0E
#define INTFB		0x0F
#define INTCAPA		0x10
#define INTCAPB		0x10
#define GPIOA		0x12
#define GPIOB		0x13
#define OLATA		0x14
#define OLATB		0x15


/*IOCON.BANK = 1 (default)*/
/*
#define IODIRA		0x00
#define IODIRB		0x10
#define IPOLA		0x01
#define IPOLB		0x11
#define GPINTENA	0x02
#define GPINTENB	0x12
#define DEFVALA		0x03
#define DEFVALB		0x13
#define INTCONA		0x04
#define INTCONB		0x14
#define IOCONA		0x05
#define IOCONB		0x15
#define GPPUA		0x06
#define GPPUB		0x16
#define INTFA		0x07
#define INTFB		0x17
#define INTCAPA		0x08
#define INTCAPB		0x18
#define GPIOA		0x09
#define GPIOB		0x19
#define OLATA		0x0A
#define OLATB		0x1A*/


U_TWI_MSG MCP_Swisspi;

U_InOut McpCtrl;

char uartMsg[100];

/*valueIOCON = 0x00 -> IOCON.BANK = 0*/
/*valueIOCON = 0x80 -> IOCON.BANK = 1*/
void MCP_INIT(/*uint8_t valueIOCON*/)
{	
	/*START-UP delay*/
	_delay_ms(10);
	
	/*Slave address and bit write*/
	MCP_Swisspi.S_MsgSplit.SlaveAdress = SLAVE_ADDR;
	MCP_Swisspi.S_MsgSplit.ReadWriteBit = TWI_WRITE;
	
	/*MCP_Swisspi.S_MsgSplit.Message[0] =	IOCON;
	MCP_Swisspi.S_MsgSplit.Message[1] =	valueIOCON;
	TWIMaster_SendMsg(MCP_Swisspi.MsgBuffer, 3);
	while(TWI_isBusy());*/
}

void MCP_GPIOA(uint8_t IOA)
{
	MCP_Swisspi.S_MsgSplit.Message[0] =	IODIRA;
	MCP_Swisspi.S_MsgSplit.Message[1] =	IOA;
	TWIMaster_SendMsg(MCP_Swisspi.MsgBuffer, 3);
	while(TWI_isBusy());
}
void MCP_GPIOB(uint8_t IOB)
{
	MCP_Swisspi.S_MsgSplit.Message[0] =	IODIRB;
	MCP_Swisspi.S_MsgSplit.Message[1] =	IOB;
	TWIMaster_SendMsg(MCP_Swisspi.MsgBuffer, 3);
	while(TWI_isBusy());
}

void MCP_OUT(uint16_t valueOut)
{
	uint8_t high = (valueOut >> 8) & 0xFF;
	uint8_t low = valueOut & 0xFF;
	
	MCP_Swisspi.S_MsgSplit.Message[0] =	GPIOA;
	MCP_Swisspi.S_MsgSplit.Message[1] = low;
	TWIMaster_SendMsg(MCP_Swisspi.MsgBuffer, 3);
	while(TWI_isBusy());
	
	MCP_Swisspi.S_MsgSplit.Message[0] =	GPIOB;
	MCP_Swisspi.S_MsgSplit.Message[1] =	high;
	TWIMaster_SendMsg(MCP_Swisspi.MsgBuffer, 3);
	while(TWI_isBusy());
}

uint16_t MCP_IN(void)
{	
	MCP_Swisspi.S_MsgSplit.ReadWriteBit = TWI_READ;
	
	MCP_Swisspi.S_MsgSplit.Message[0] =	GPIOA;
	TWIMaster_SendMsg(MCP_Swisspi.MsgBuffer, 3);
	while(TWI_isBusy());
	
	TWI_GetData(MCP_Swisspi.MsgBuffer, 3);
	
	MCP_Swisspi.S_MsgSplit.ReadWriteBit = TWI_WRITE;
	
	return MCP_Swisspi.MsgBuffer[2];
}