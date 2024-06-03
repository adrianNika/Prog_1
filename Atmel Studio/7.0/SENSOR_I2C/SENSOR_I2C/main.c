/*
 * SENSOR_I2C.c
 *
 * Created: 17/02/2023 10:40:53
 * Author : Fix-5
 */ 


#define F_CPU 16000000UL
#include <avr/io.h>
#include "AVR_Uart.h"
#include "TWI_Master.h"
#include <util/delay.h>
#include <stdint-gcc.h>
#include <string.h>


#define ADDR_PCF8574 0x25

int main(void)
{
	/**/
	DDRC = 0x20;
	
	/*
	nous allumons les LEDs quand la sortie passe a l'état bas car comme donner dans
	la Datasheet le courant fournit a l'état haut n'est pas plus grand que 300uA
	ce qui n'est pas suffisant pour allumer les LEDs.
	nous les utilisons a l'état bas, ils peuvent prendre jusqu'a 25mA dans les I/O
	courant dans les IO i = vcc / R = 5 / 330 = 15mA
	
	les valeur binaire des led sont 0 1 2 4 8 16 32 64 128 a l'etat haut active
	
	0 -> 255
	1 -> 254
	2 -> 253
	4 -> 251
	8 -> 247
	16 -> 239
	32 -> 223
	64 -> 191
	128 -> 127
	
	nous somme a l'etat bas active
	*/
	TWIMaster_Init();
	UART_Init();
	
	U_TWI_MSG message;

	message.S_MsgSplit.SlaveAdress = ADDR_PCF8574;
	message.S_MsgSplit.ReadWriteBit = TWI_WRITE;
	
	
	sei();
	
    /* Replace with your application code */
    while (1) 
    {	
		/*k2000 incremente*/
		for (int i = 1; i < 128; i*=2)
		{
			message.S_MsgSplit.Message[0] = ~i;
			TWIMaster_SendMsg(message.MsgBuffer , 2);
			while(TWI_isBusy());
			_delay_ms(25);
		}
		
		/*k2000 décremente*/
 		for (int i = 128; i > 1; i/=2)
 		{
			 message.S_MsgSplit.Message[0] = ~i;
			 TWIMaster_SendMsg(message.MsgBuffer , 2);
			 while(TWI_isBusy());
	 		_delay_ms(25);
 		}
    }
}

