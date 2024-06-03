/*
 * Duellator_V1.c
 *
 * Created: 16/05/2023 07:45:31
 * Author : Fix-5
 */ 
#include "global.h"

U_TWI_MSG led;

uint16_t tON = 0;
uint8_t game = 0;
uint8_t gameSelect = 0;
uint8_t players = 0;
uint8_t PlayersSelect = 0;
uint8_t flag_ready_p1 = 0;
uint8_t flag_ready_p2 = 0;
uint8_t flag = 0;
char *p1 = "";
char *p2 = "";

typedef enum
{
	STATE_MENU,
	STATE_GAMES_SELECT,
	STATE_PLAYERS_SELECT,
	STATE_GET_READY,
	NO_STATE
}E_STATE_LCD;

E_STATE_LCD etatLCD_AFF = STATE_MENU;

int main(void)
{
	DDRB = 0x00;
	
	char *listeGames[] = {"REFLEX", "FASTPULSE", "PONG", "PERFECT"};
	char *listePlayers[] = {"BOA", 
							"DUI",
							"SCJ",
							"PED",
							"STT",
							"MOR",
							"LEM",
							"PRN",
							"PRG",
							"DUJ",
							"BUN",
							"BLN",
							"BUE",
							"TOG"};
	
	sei();
	
	TWIMaster_Init();
	UART_Init();
	LCD_Init(0x09, 0x25);
	LCD_Clear();
	
	led.S_MsgSplit.SlaveAdress = SLAVEADDR_PCF8574A;
	led.S_MsgSplit.ReadWriteBit = TWI_WRITE;
	
	for (int i = 0; i < 10; i++)
	{
		led.S_MsgSplit.Message[0] = 170;
		TWIMaster_SendMsg(led.MsgBuffer , 2);
		while(TWI_isBusy());
		_delay_ms(100);
		led.S_MsgSplit.Message[0] = 85;
		TWIMaster_SendMsg(led.MsgBuffer , 2);
		while(TWI_isBusy());
		_delay_ms(100);	
	}
	
	led.S_MsgSplit.Message[0] = 255;
	TWIMaster_SendMsg(led.MsgBuffer , 2);
	while(TWI_isBusy());
	
	
	
    /* Replace with your application code */
    while (1) 
    {
		switch (etatLCD_AFF)
		{
			case STATE_MENU:
				LCD_Clear();
				LCD_setCursor(0,0);
				LCD_PutString("------MENU------");
				LCD_setCursor(1,0);
				LCD_PutString("GAME: ");
				LCD_PutString(listeGames[game]);
				
				etatLCD_AFF = NO_STATE;
			break;
			
			case STATE_GAMES_SELECT:
				gameSelect = 1;
				LCD_Clear();
				LCD_setCursor(0,0);
				LCD_PutString(listeGames[game]);
				
				_delay_ms(3000);
				
				etatLCD_AFF = STATE_PLAYERS_SELECT;
			break;
			
			case STATE_PLAYERS_SELECT:
				LCD_Clear();
				LCD_setCursor(0,0);
				if (PlayersSelect == 0)
				{
					LCD_PutString("Name P1: ");
					LCD_PutString(listePlayers[players]);
				}
				else if (PlayersSelect == 1)
				{
					LCD_PutString("Name P2: ");
					LCD_PutString(listePlayers[players]);
				}
				
				etatLCD_AFF = NO_STATE;
			break;
				
			case STATE_GET_READY:
				LCD_Clear();
				LCD_setCursor(0,0);
				LCD_PutString("--Press Start--");
				LCD_setCursor(1,0);
				
				LCD_PutString(p1);
				if (PlayersSelect == 3)
				{
					flag_ready_p1 = 1;
					PlayersSelect = 5;
				}
				if (flag_ready_p1 == 1)
				{
					LCD_PutString(": OK  ");
				}
				else
				{
					LCD_PutString(": NO  ");
				}
				
				LCD_PutString(p2);
				if (PlayersSelect == 4)
				{
					flag_ready_p2 = 1;
					PlayersSelect = 5;
				}
				if (flag_ready_p2 == 1)
				{
					LCD_PutString(": OK");
				}
				else
				{
					LCD_PutString(": NO");
				}
					
				etatLCD_AFF = NO_STATE;
			break;
			
			default:
				if (gameSelect == 0)
				{
					while (PINB & (1 << PINB0))
					{
						tON += 1;
						_delay_ms(1);
					}

					if (tON <= 500 && tON > 0)
					{
						game += 1;
						if (game == 4)
						{
							game = 0;
						}
						etatLCD_AFF = STATE_MENU;
					}
					else if (tON >= 1000)
					{
						etatLCD_AFF = STATE_GAMES_SELECT;
					}
					
					tON = 0;
				}
				else
				{
					if (PlayersSelect < 2)
					{
						if (PlayersSelect == 0)
						{
							while (PINB & (1 << PINB0))
							{
								tON += 1;
								_delay_ms(1);
							}
						}
						if (PlayersSelect == 1)
						{
							while (PINB & (1 << PINB1))
							{
								tON += 1;
								_delay_ms(1);
							}
						}

						if (tON <= 500 && tON > 0)
						{
							players += 1;
							if (players == 14)
							{
								players = 0;
							}
							etatLCD_AFF = STATE_PLAYERS_SELECT;
						}
						else if (tON >= 500)
						{
							PlayersSelect += 1;
							if (PlayersSelect == 1)
							{
								p1 = listePlayers[players];
								etatLCD_AFF = STATE_PLAYERS_SELECT;
							}
							
							if (PlayersSelect == 2)
							{
								p2 = listePlayers[players];
								etatLCD_AFF = STATE_GET_READY;
							}	
							
							players = 0;					
						}
						
						tON = 0;
					}
					else
					{
						while (PINB & (1 << PINB0))
						{
							PlayersSelect = 3;
						}
						if (PlayersSelect == 3)
						{
							etatLCD_AFF = STATE_GET_READY;
						}
						
						while (PINB & (1 << PINB1))
						{
							PlayersSelect = 4;
						}
						if (PlayersSelect == 4)
						{
							etatLCD_AFF = STATE_GET_READY;
						}
					}	
				}
			
			break;
		}
    }
}

