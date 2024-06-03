/*
 * TEST_TDUS.c
 *
 * Created: 19/04/2023 15:29:34
 * Author : Fix-5
 */ 

#define F_CPU 16000000
#include <avr/io.h>

typedef union
{
	struct
	{
		uint8_t zero:1;
		uint8_t un:1;
		uint8_t deux:1;
		uint8_t trois:1;
		uint8_t quatre:1;
		uint8_t cinq:1;
		uint8_t six:1;
		uint8_t sept:1;
	}S_output;
	
	uint8_t byte;
}U_output;

U_output output;

typedef union
{
	struct
	{
		uint8_t zero:1;
		uint8_t un:1;
		uint8_t deux:1;
		uint8_t trois:1;
		uint8_t quatre:1;
		uint8_t cinq:1;
		uint8_t six:1;
		uint8_t sept:1;
	}S_input;
	
	uint8_t byte;
}U_input;

U_input input;

void sortie (int valeur)
{
	PORTC = valeur & 0x0F;
	PORTD = (valeur & 0xF0);
}

int main(void)
{
	DDRC = 0x0F;
	DDRD = 0xF0;
	DDRB = 0x00;
	
    /* Replace with your application code */
    while (1) 
    {
		input.byte = PINB;
		
		if (input.S_input.zero == 0x01)
		{
			output.byte = 0x0F;
		}
		
		if (input.S_input.un == 0x01)
		{
			output.byte = 0xF0;
		}
		
		sortie(output.byte);
    }
}

