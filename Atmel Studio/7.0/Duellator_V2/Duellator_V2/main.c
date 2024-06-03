/*
 * Duellator_V2.c
 *
 * Created: 16/05/2023 07:45:31
 * Author : Fix-5
 */ 
#include "global.h"

U_TWI_MSG led;

uint16_t tON = 0;
uint16_t tON2 = 0;
uint8_t names = 0;
uint16_t scoreP1 = 0x0000;
uint16_t scoreP2 = 0x0000;
uint16_t score = 0;
uint16_t bestScoreReflex = 0x0000;
uint8_t addrEEPROM = 0x00;
uint8_t winner = 0;

char *p1 = "";
char *p2 = "";

char i2cMsg[] = "";

uint8_t ledStart[] = {252,240,192,0};
uint8_t ledON[] = {255,126,60,24,0,24,60,126};
	
char no[] = "NO";
char ok[] = "OK";
	
char *listePlayers[] = {"BOA", "SCJ", "DUI", "PED",
						"STT", "MOR", "LEM", "NOP",
						"GOP", "DUJ", "BUN", "BLN",
						"BUE", "TOG", " "," "};

typedef struct
{
	uint16_t flagP1ShortPulse:1;
	uint16_t flagP1LongPulse:1;
	uint16_t flagP2ShortPulse:1;
	uint16_t flagP2LongPulse:1;
	uint16_t flagStartGame:1;
	uint16_t flagDisplayRestart:1;
	uint16_t restart:1;
	uint16_t flagUpdateDisplay:1;
	uint16_t p1_Select:1;
	uint16_t p2_Select:1;
	uint16_t flagReadyP1:1;
	uint16_t flagReadyP2:1;
	uint16_t wrongStart:1;
}S_flag;


typedef enum
{
	STATE_SELECT_GAME,
	STATE_SELECT_NAMES,
	STATE_WAIT_READY,
	STATE_GAME1_PLAYING,
	STATE_GAME2_PLAYING,
	STATE_GAME3_PLAYING,
	STATE_GAME4_PLAYING,
	STATE_SHOW_WINNER,
	STATE_BEST_SCORE_BATTUE,
	STATE_SHOW_SCORE
}E_SM_GLOBAL;


typedef enum
{
	SELECT_GAME_1,
	SELECT_GAME_2,
	SELECT_GAME_3,
	SELECT_GAME_4
}E_GAME_STATE;


typedef enum
{
	SELECT_PLAYER1,
	SELECT_PLAYER2
}E_SELECT_PLAYER;

typedef enum
{
	BEST_SCORE,
	SCORE_PLAYERS_START,
	PLAY,
	WRONG_START
}E_DUELLATOR;

typedef enum
{
	TIMER_FOR_PULSE,
	TIMER_FOR_SCORE,
}E_TIMER;

E_SM_GLOBAL global = STATE_SELECT_GAME;
E_SM_GLOBAL games_selected = STATE_GAME1_PLAYING;
E_GAME_STATE StateGameSelect = SELECT_GAME_1;
E_SELECT_PLAYER statePlayerSelect = SELECT_PLAYER1;
E_DUELLATOR duellator = BEST_SCORE;
E_TIMER timer = TIMER_FOR_PULSE;
S_flag flag;


ISR (PCINT2_vect)
{
	if (PIND & ~(1 << PIND4))
	{
		if (tON <= 400 && tON > 0)
		{
			flag.flagP1ShortPulse = 1;
		}
		else if (tON > 400)
		{
			flag.flagP1LongPulse = 1;
		}
		
		tON = 0;
		TCCR1B &= ~(1 << CS10);
		TCNT1 = 0;
		flag.p1_Select = 0;
	}
	
	if (PIND & ~(1 << PIND5))
	{
		if (tON2 <= 400 && tON2 > 0)
		{
			flag.flagP2ShortPulse = 1;
		}
		else if (tON2 > 400)
		{
			flag.flagP2LongPulse = 1;
		}
		
		tON2 = 0;
		TCCR1B &= ~(1 << CS10);
		TCNT1 = 0;
		flag.p2_Select = 0;
	}
	
	if ((PIND & (1 << PIND4)) || (PIND & (1 << PIND5)))
	{
		TCCR1B |= (1 << CS10);
		timer = TIMER_FOR_PULSE;
		
		if (PIND & (1 << PIND4))
		{
			flag.p1_Select = 1;
		}
		if (PIND & (1 << PIND5))
		{
			flag.p2_Select = 1;
		}
	}
}

ISR (TIMER1_COMPA_vect)
{
	
	switch (timer)
	{
		case TIMER_FOR_PULSE:
		
			if (flag.p1_Select != 0)
			{
				tON++;
			}
			else if (flag.p2_Select != 0)
			{
				tON2++;
			}
			
		break;
		
		case TIMER_FOR_SCORE:
			score++;
			PORTC ^= 0x01;
		break;
		
		default:
		break;
	}	
}


uint16_t giveArandomDelay(uint16_t score)
{
	uint16_t delay = 0;
	uint16_t rdmDelay[32] = {450,864,321,941,8,69,210,478,1400,1699,632,285,
							 102,368,1900,985,52,39,750,621,52,765,899,988,
							 1268,200,300,389,486,599,1468,233};
	 delay = score % 32;
	 
	 return rdmDelay[delay];
}


void delay(uint16_t milliseconds)
{
	uint16_t iterations = (F_CPU / 10000) * milliseconds;
	
	for (uint16_t i = 0; i < iterations; i++) {
		asm volatile("nop");
	}
}

void init_interPCINT(void)
{
	PCICR |= (1 << PCIE2);
}

void init_Timer1(void)
{
	TCCR1A |= 0x00;
	TCCR1B |= (1 << WGM12);
	TIMSK1 |= (1 << OCIE1A);
	OCR1A = 7999;
}

void init_WDT(void)
{
	wdt_reset();
	WDTCSR |= (1 << WDCE) | (1 << WDE);
	WDTCSR = ((1 << WDIE) | (1 << WDE) | (1 << WDP3) | (1 << WDP0));
}

void bliking_Start(void)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			wdt_reset();
			led.S_MsgSplit.Message[0] = ledON[j];
			TWIMaster_SendMsg(led.MsgBuffer , 2);
			while(TWI_isBusy());
			PORTD ^= 0xC0;
			_delay_ms(80);
		}
	}
	
	led.S_MsgSplit.Message[0] = 255;
	TWIMaster_SendMsg(led.MsgBuffer , 2);
	while(TWI_isBusy());
}

int main(void)
{
	DDRB = 0x00;
	DDRC = 0x01;
	DDRD = 0xC0;
	
	sei();
	TWIMaster_Init();
	
	UART_Init();
	
	init_interPCINT();

	init_Timer1();
	
	init_WDT();
	
	LCD_Init(0x09, 0x25);
	LCD_Clear();
	
	led.S_MsgSplit.SlaveAdress = SLAVEADDR_PCF8574A;
	led.S_MsgSplit.ReadWriteBit = TWI_WRITE;
	
	bliking_Start();
	
	flag.flagUpdateDisplay = 1;
	
	Uart_PutString("OK");

    /* Replace with your application code */
    while (1) 
    {
		switch (global)
		{
			case STATE_SELECT_GAME:
			
				PCMSK2 = (1 << PCINT20);
				
				if (flag.flagP1ShortPulse != 0)
				{
					flag.flagP1ShortPulse = 0;
					flag.flagP2ShortPulse = 0;
					flag.flagUpdateDisplay = 0x01; 
					 
					switch (StateGameSelect)
					{
						case SELECT_GAME_1:
							games_selected = STATE_GAME1_PLAYING;
						break;
						
						case SELECT_GAME_2:
							games_selected = STATE_GAME2_PLAYING;
						break;
						
						case SELECT_GAME_3:
							games_selected = STATE_GAME3_PLAYING;
						break;
						
						case SELECT_GAME_4:
							games_selected = STATE_GAME4_PLAYING;
						break;
						
						default:
						break;
					}
				}
				else
				{
					wdt_reset();
				}
				if (flag.flagP1LongPulse != 0)
				{
					scoreP1 = 0;
					scoreP2 = 0;
					flag.flagP1LongPulse = 0;
					flag.flagP2LongPulse = 0;
					flag.flagUpdateDisplay = 0x01;
					global = STATE_SELECT_NAMES;
				} 
			break;
			
			case STATE_SELECT_NAMES:
				switch (statePlayerSelect)
				{
					case SELECT_PLAYER1:
					
						PCMSK2 = (1 << PCINT20);
						
						if (flag.flagP1ShortPulse != 0)
						{
							wdt_reset();
							flag.flagP1ShortPulse = 0;
							flag.flagUpdateDisplay = 0x01;
							
							names += 1;
							if (names == 14)
							{
								names = 0;
							}
						}
						
						if (flag.flagP1LongPulse != 0)
						{
							wdt_reset();
							p1 = listePlayers[names];
							flag.flagUpdateDisplay = 0x01;
							flag.flagP1LongPulse = 0;
							names = 0;
							statePlayerSelect = SELECT_PLAYER2;	
						}
						
					break;
					
					case SELECT_PLAYER2:
					
						PCMSK2 = (1 << PCINT21);
						
						if (flag.flagP2ShortPulse)
						{
							wdt_reset();
							flag.flagP2ShortPulse = 0;
							flag.flagUpdateDisplay = 0x01;
							
							names += 1;
							if (names == 14)
							{
								names = 0;
							}
						}
						
						if (flag.flagP2LongPulse != 0)
						{
							wdt_reset();
							flag.flagP2LongPulse = 0;
							
							if (listePlayers[names] != p1)
							{
								p2 = listePlayers[names];
								flag.flagUpdateDisplay = 0x01;
								flag.flagP2LongPulse = 0;
								flag.flagP2ShortPulse = 0;
								names = 0;
								global = STATE_WAIT_READY;
							}
						}
						
					break;
					
					default:
					break;
				}
			break;
			
			case STATE_WAIT_READY:
			
				PCMSK2 |= ((1 << PCINT20) | (1 << PCINT21));
				if (flag.flagP1ShortPulse != 0 && flag.flagReadyP1 == 0)
				{
					wdt_reset();
					flag.flagUpdateDisplay = 1;
					flag.flagReadyP1 = 1;
				}
				
				if (flag.flagP2ShortPulse != 0 && flag.flagReadyP2 == 0)
				{
					wdt_reset();
					flag.flagUpdateDisplay = 1;
					flag.flagReadyP2 = 1;
				}
				
				if (flag.restart != 0)
				{
					if (flag.flagP1LongPulse != 0 || flag.flagP2LongPulse != 0)
					{
						flag.flagDisplayRestart = 0;
						flag.flagUpdateDisplay = 1;
						flag.flagP1LongPulse = 0;
						flag.flagP2LongPulse = 0;
						flag.flagP2ShortPulse = 0;
						flag.flagP1ShortPulse = 0;
						flag.flagReadyP1 = 0;
						flag.flagReadyP2 = 0;
						flag.restart = 0;
						scoreP1 = 0;
						scoreP2 = 0;
						PCMSK2 = 0;
						
						global = STATE_SELECT_GAME;
						statePlayerSelect = SELECT_PLAYER1;
						StateGameSelect = SELECT_GAME_1;
					}	
				}
				
			break;
			
			case STATE_GAME1_PLAYING:
			
				switch (duellator)
				{
					case BEST_SCORE:
						/**/
					break;
				
					case SCORE_PLAYERS_START:
					
						if (flag.flagStartGame != 0)
						{
							wdt_reset();
							flag.flagStartGame = 0;
							PCMSK2 &= ~((1 << PCINT20) | (1 << PCINT21));
							
							for (int i = 0; i < 4; i++)
							{
								wdt_reset();
								
								led.S_MsgSplit.Message[0] = ledStart[i];
								TWIMaster_SendMsg(led.MsgBuffer , 2);
								while(TWI_isBusy());
								
								for (uint8_t y = 0; y < 251; y++)
								{
									if (PIND & (1 << PIND4))
									{
										duellator = WRONG_START;
										flag.wrongStart = 1;
										flag.p1_Select = 1;
										break;
									}
									if (PIND & (1 << PIND5))
									{
										duellator = WRONG_START;
										flag.wrongStart = 1;
										flag.p2_Select = 1;
										break;
									}
									_delay_ms(4);
								}
							}
							
							if (flag.wrongStart != 1)
							{	
								delay(giveArandomDelay(scoreP1));
								wdt_reset();
								
								led.S_MsgSplit.Message[0] = 255;
								TWIMaster_SendMsg(led.MsgBuffer , 2);
								while(TWI_isBusy());
								
								score = 0;
								
								PORTD = 0x00;
								
								timer = TIMER_FOR_SCORE;
								duellator = PLAY;
								TCCR1B |= (1 << CS10);
							}
						}
						
					break;
				
					case PLAY:
					
						if ((PIND & (1 << PIND4)) && (flag.p1_Select != 1))
						{
							scoreP1 = score;
							flag.p1_Select = 1;
						}
						
						if ((PIND & (1 << PIND5)) && (flag.p2_Select != 1))
						{
							scoreP2 = score;
							flag.p2_Select  = 1;
						}
						
						if ((flag.p1_Select != 0) && (flag.p2_Select != 0))
						{
							flag.p1_Select = 0;
							flag.p2_Select = 0;
							score = 0;
							TCCR1B &= ~(1 << CS10);
							TCNT1 = 0;
							
							while (PIND & (1 << PIND4) || PIND & (1 << PIND5));
							global = STATE_SHOW_WINNER;
						}
						
					break;
					
					case WRONG_START:
						flag.flagUpdateDisplay = 1;
						flag.wrongStart = 0;
					break;
					
					default:
					break;
				}
			break;
			
			case STATE_GAME2_PLAYING:
				
			break;
			
			case STATE_GAME3_PLAYING:
				
			break;
			
			case STATE_GAME4_PLAYING:
				
			break;
			
			case STATE_SHOW_WINNER:
			
				if (scoreP1 < scoreP2)
				{
					winner = 1;
				}
				else if (scoreP2 < scoreP1)
				{
					winner = 2;
				}
				else
				{
					winner = 0;
				}
				
				flag.flagUpdateDisplay = 0x01;
				
			break;
			
			case STATE_BEST_SCORE_BATTUE:
				/**/
			break;
			 
			case STATE_SHOW_SCORE:		
				/**/
			break;
			
			default:
			break;
		}
		
		
/***************************************SHWO DISPLAY*******************************************/

		if(flag.flagUpdateDisplay != 0)
		{
			flag.flagUpdateDisplay = 0;
			
			switch(global)
			{
				case STATE_SELECT_GAME:
				
					LCD_Clear();
					LCD_setCursor(0,0);
					LCD_PutString("------MENU------");
					LCD_setCursor(1,0);
					LCD_PutString("GAME: ");
					
					switch(StateGameSelect)
					{
						case SELECT_GAME_1:
							LCD_PutString("REFLEX");
							StateGameSelect = SELECT_GAME_2;
						break;
					 
						case SELECT_GAME_2:
							LCD_PutString("FAST");
							StateGameSelect = SELECT_GAME_3;
						break;
					 
						case SELECT_GAME_3:
							LCD_PutString("PONG");
							StateGameSelect = SELECT_GAME_4;
						break;
					 
						case SELECT_GAME_4:
							LCD_PutString("PERFECT");
							StateGameSelect = SELECT_GAME_1;
						break;
					 
						default:
						break;
					}
				 break;
				 
				 case STATE_SELECT_NAMES:
				 
					LCD_Clear();
					LCD_setCursor(0,0);
					
					switch (statePlayerSelect)
					{
						case SELECT_PLAYER1:
							LCD_PutString("SELECT Name P1:");
							LCD_setCursor(1,1);
							LCD_PutString(">");
							LCD_PutString(listePlayers[names]);
							LCD_PutString("< ");
							LCD_PutString(listePlayers[names+1]);
							LCD_PutString(" ");
							LCD_PutString(listePlayers[names+2]);
						break;
						
						case SELECT_PLAYER2:
							LCD_PutString("SELECT Name P2:");
							LCD_setCursor(1,1);
							LCD_PutString(">");
							LCD_PutString(listePlayers[names]);
							LCD_PutString("< ");
							LCD_PutString(listePlayers[names+1]);
							LCD_PutString(" ");
							LCD_PutString(listePlayers[names+2]);
						break;
						
						default:
						break;
					}
					
				 break;
				 
				 case STATE_WAIT_READY:
				 
					LCD_Clear();
					LCD_setCursor(0,0);
					
					if (flag.flagDisplayRestart == 0)
					{
						LCD_PutString("-----READY?-----");
					}
					else
					{
						LCD_PutString("Press to restart");
						flag.restart = 1;
						flag.flagStartGame = 0;
					}
					
					LCD_setCursor(1,0);
					
					LCD_PutString(p1);
					LCD_PutString(": ");
					if (flag.flagP1ShortPulse != 0)
					{
						LCD_PutString(ok);
					}
					else
					{
						LCD_PutString(no);
					}
					
					LCD_PutString("  ");
					LCD_PutString(p2);
					LCD_PutString(": ");
					if (flag.flagP2ShortPulse != 0)
					{
						LCD_PutString(ok);
					}
					else
					{
						LCD_PutString(no);
					}
					
					if (flag.flagP1ShortPulse != 0 && flag.flagP2ShortPulse != 0)
					{
						global = games_selected;
						PCMSK2 &= ~((1 << PCINT20) | (1 << PCINT21));
						flag.flagDisplayRestart = 0;
						flag.flagP2ShortPulse = 0;
						flag.flagP1ShortPulse = 0;
						flag.flagUpdateDisplay = 1;
						flag.restart = 0;
						
						switch (games_selected)
						{
							case STATE_GAME1_PLAYING:
							
								flag.flagDisplayRestart = 0;
								flag.flagP2LongPulse = 0;
								flag.flagP1LongPulse = 0;
								flag.flagStartGame = 0;
								flag.flagReadyP1 = 0;
								flag.flagReadyP2 = 0;
								flag.p2_Select = 0;
								flag.p1_Select = 0;
								duellator = BEST_SCORE;
								
							break;
							
							case STATE_GAME2_PLAYING:
							break;
							
							case STATE_GAME3_PLAYING:
							break;
							
							case STATE_GAME4_PLAYING:
							break;
							
							default:
							break;
						}
						
						wdt_reset();
						_delay_ms(2000);
						wdt_reset();
					}
					
				 break;
				 
				 case STATE_GAME1_PLAYING:
					switch (duellator)
					{
						case BEST_SCORE:
						
							LCD_Clear();
							LCD_setCursor(0,0);
							LCD_PutString("-THE GAME START-");
							LCD_setCursor(1,0);
							LCD_PutString("BEST SCORE: ");
							
							bestScoreReflex = (eeprom_read_byte((uint8_t *)0) << 8);
							bestScoreReflex = bestScoreReflex  + eeprom_read_byte((uint8_t *)1);
							
							sprintf(i2cMsg, "%4d", bestScoreReflex);
							LCD_PutString(i2cMsg);
							
							wdt_reset();
							_delay_ms(3000);
							wdt_reset();
							
							flag.flagUpdateDisplay = 0x01;
							duellator = SCORE_PLAYERS_START;
							
						break;
						
						case SCORE_PLAYERS_START:
						
							LCD_Clear();
							LCD_setCursor(0,0);
							LCD_PutString(p1);
							sprintf(i2cMsg, ": %d", scoreP1);
							LCD_PutString(i2cMsg);
							
							LCD_setCursor(1,0);
							LCD_PutString(p2);
							sprintf(i2cMsg, ": %d", scoreP2);
							LCD_PutString(i2cMsg);
							
							wdt_reset();
							_delay_ms(1000);
							
							flag.flagStartGame = 1;
						break;
						
						case PLAY:
							/**/
						break;
						
						case WRONG_START:
						
							LCD_Clear();
							LCD_setCursor(0,0);
							LCD_PutString("--Wrong start!--");
							LCD_setCursor(1,7);
							
							if (flag.p1_Select != 0)
							{
								flag.p1_Select = 0;
								LCD_PutString(p1);
							}
							else if (flag.p2_Select != 0)
							{
								flag.p2_Select = 0;
								LCD_PutString(p2);
							}
							
							for (int b = 0; b < 13; b++)
							{
								led.S_MsgSplit.Message[0] = 0;
								TWIMaster_SendMsg(led.MsgBuffer , 2);
								while(TWI_isBusy());
								_delay_ms(80);
								led.S_MsgSplit.Message[0] = 255;
								TWIMaster_SendMsg(led.MsgBuffer , 2);
								while(TWI_isBusy());
								_delay_ms(80);
								wdt_reset();
							}
							
							wdt_reset();
							
							PCMSK2 |= ((1 << PCINT20) | (1 << PCINT21));
							PCICR |= (1 << PCIE2);
							flag.flagUpdateDisplay = 1;
							flag.flagP1ShortPulse = 0;
							flag.flagP2ShortPulse = 0;
							flag.flagP1LongPulse = 0;
							flag.flagP2LongPulse = 0;
							flag.flagDisplayRestart = 1;

							global = STATE_WAIT_READY;	
													
						break;
						
						default:
						break;
					}
				 break;
				 
				 case STATE_GAME2_PLAYING:
					LCD_Clear();
					LCD_PutString("GAME NO CREATED");
					while(1);
				 break;
				 
				 case STATE_GAME3_PLAYING:
					LCD_Clear();
					LCD_PutString("GAME NO CREATED");
					while(1);
				 break;
				 
				 case STATE_GAME4_PLAYING:
					LCD_Clear();
					LCD_PutString("GAME NO CREATED");
					while(1);
				 break;
				 
				 case STATE_SHOW_WINNER:
				 
					LCD_Clear();
					LCD_setCursor(0,0);
					if (winner != 0)
					{
						LCD_PutString("-----WINNER-----");
					}
					else
					{
						LCD_PutString("-----EGALITE----");
					}
					LCD_setCursor(1,0);
						
					if (winner == 1)
					{
						LCD_PutString(p1);
						LCD_PutString(": ");
						sprintf(i2cMsg, "%d", scoreP1);
						LCD_PutString(i2cMsg);
						
						for (int a = 0; a < 8; a++)
						{
							PORTD ^= 0x40;
							_delay_ms(80);
						}
					}
					else if (winner == 2)
					{
						LCD_PutString(p2);
						LCD_PutString(": ");
						sprintf(i2cMsg, "%d", scoreP2);
						LCD_PutString(i2cMsg);
						
						for (int a = 0; a < 8; a++)
						{
							PORTD ^= 0x80;
							_delay_ms(80);
						}
					}
					
					wdt_reset();
					_delay_ms(2500);
					wdt_reset();
					
					winner = 0;
					flag.flagUpdateDisplay = 1;
					
					global = STATE_BEST_SCORE_BATTUE;
					
				 break;
				 
				 case STATE_BEST_SCORE_BATTUE:
				 
					Uart_PutString("OK");
					if ((scoreP1 < bestScoreReflex) || (scoreP2 < bestScoreReflex))
					{
						eeprom_write_byte(0,0);
						if (scoreP1 < scoreP2)
						{
							eeprom_write_byte((uint8_t *)0, (scoreP1 >> 8) & 0xFF);
							eeprom_write_byte((uint8_t *)1,  scoreP1 & 0xFF);
							flag.p1_Select = 1;
						}
						else
						{
							eeprom_write_byte((uint8_t *)0, (scoreP2 >> 8) & 0xFF);
							eeprom_write_byte((uint8_t *)1,  scoreP2 & 0xFF);
							flag.p2_Select = 1;
						}
						
						bestScoreReflex = 0x0000;
						
						LCD_Clear();
						LCD_setCursor(0,0);
						LCD_PutString("Meilleurs score");
						LCD_setCursor(1,0);
						LCD_PutString("Battue par: ");
						
						if (flag.p1_Select != 0)
						{
							LCD_PutString(p1);
							flag.p1_Select = 0;
						}
						if (flag.p2_Select != 0)
						{
							LCD_PutString(p2);
							flag.p2_Select = 0;
						}
						 
						_delay_ms(3000);
					 }
					
					global = STATE_SHOW_SCORE;
		
				 break;
				 
				 case STATE_SHOW_SCORE:
						
					LCD_Clear();
					LCD_setCursor(0,0);
 					LCD_PutString("Score ");
					LCD_PutString(p1);
 					LCD_PutString(": ");						
					sprintf(i2cMsg,"%d", scoreP1);
					LCD_PutString(i2cMsg);
					
					LCD_setCursor(1,0);
					LCD_PutString("Score ");
					LCD_PutString(p2);
					LCD_PutString(": ");
					sprintf(i2cMsg,"%d", scoreP2);
					LCD_PutString(i2cMsg);
					
					wdt_reset();
					_delay_ms(3000);
					wdt_reset();
					
					global = STATE_WAIT_READY;
					PCMSK2 |= ((1 << PCINT20) | (1 << PCINT21));
					PCICR |= (1 << PCIE2);
					flag.flagUpdateDisplay = 1;
					flag.flagReadyP1 = 0;
					flag.flagReadyP2 = 0;
					flag.flagDisplayRestart = 1;
					
				 break;
				 
				 default:
				 break;
			 }
		 }
	}
}