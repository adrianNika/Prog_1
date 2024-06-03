/*
 * Swiss_PI.c
 *
 * Created: 27/03/2023 11:12:03
 * Author : Fix-5
 *
 * main pour librairie MCP 23017 en I2C sur swiss pi
 *
 *
 */ 

#include "MCP_23017_I2C.h"

#define SLAVE_ADDR 0x20

U_TWI_MSG MCP_Swisspi;
U_InOut McpCtrl;

int main(void)
{
	uint16_t lecture = 0;
	
	sei();
	
	TWIMaster_Init();
	
	MCP_INIT(/*0x00*/);
	
	UART_Init();
	
	MCP_GPIOA(0xFF);
	MCP_GPIOB(0x00);
	
	MCP_OUT(0x5500);
	
	while (1)
    {	
		lecture = MCP_IN();
		
		MCP_OUT(lecture);
		
		while(1);
	}
}