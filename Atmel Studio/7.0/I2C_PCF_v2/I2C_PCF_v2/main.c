/*
 * I2C_PCF_v2.c
 *
 * Created: 21/02/2023 15:44:36
 * Author : Fix-5
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include "AVR_Uart.h"
#include "TWI_Master.h"
#include <util/delay.h>
#include <stdint-gcc.h>
#include <string.h>

/*la valeur de registre se trouve dans la datasheet TI page 9/42 sous slaveAdress*/
#define ADDR_PCF8574 0x20

int main(void)
{
	/**/
	DDRC = 0x20;
	
	uint8_t flag = 0;
	uint8_t k2000 = 1;
	
	TWIMaster_Init();
	UART_Init();
	
	U_TWI_MSG message;

	message.S_MsgSplit.SlaveAdress = ADDR_PCF8574;
	message.S_MsgSplit.ReadWriteBit = TWI_WRITE;
	
	
	sei();
	
    /* Replace with your application code */
    while (1) 
    {	
		/*
		nous allumons les LEDs quand la sortie passe a l'état bas car comme donner dans
		la Datasheet le courant fournit a l'état haut n'est pas plus grand que 300uA
		ce qui n'est pas suffisant pour allumer les LEDs.
		nous les utilisons a l'état bas, ils peuvent prendre jusqu'a 25mA dans les I/O
		courant dans les IO i = vcc / R = 5 / 330 = 15mA
		
		les valeur binaire des led sont 0 1 2 4 8 16 32 64 128 a l'etat haut active
		
		0 ~= 255
		1 ~= 254
		2 ~= 253
		4 ~= 251
		8 ~= 247
		16 ~= 239
		32 ~= 223
		64 ~= 191
		128 ~= 127
		
		nous somme a l'etat bas active
		donc nous pouvons faire une inversion par complement a 1 avec le signe "~"
		*/
		message.S_MsgSplit.Message[0] = ~k2000;
		TWIMaster_SendMsg(message.MsgBuffer , 2);
		while(TWI_isBusy());
			
		 if (k2000 != 128 && flag == 0)
		 {
			 k2000 *= 2;
		 }
		 else
		 {
			 flag = 1;
		 }
		 if (k2000 != 1 && flag == 1)
		 {
			 k2000 /= 2;
		 }
		 else
		 {
			 flag = 0;
		 }
		
	 	_delay_ms(25);
    }
}