/*
 * Duellator_V2_Final.c
 *
 * Created: 16/05/2023 07:45:31
 * Author : Fix-5
 *	
 * Projets: Le duellator est un appareils comportant un jeux de reflex a 2J
 *          il y a un menu défilent pour les jeux (pour l'instant que jeux
 *          Reflex disponible) court pulse pour changer de jeux et longue 
 *			pulse pour séléctionner le jeux, aprés séléction du jeux vous 
 *			allez choisire vos nom court pulse pour changer de nom et longue
 *			pulse pour séléctionner le nom.
 *			apres a vous de mettre OK quand vous etes près (attention après
 *			8 secondes le jeux vas rédemarer donc mettez vous vite près)
 *			quand vous êtes prêt le jeux se lance, d'habord ou affiche le 
 *			Best Score serra afficher apres les enciens scores des 2 joueurs
 *			s'affiche et après 1 seconde le decompte Leds commence, si il y 
 *			un appuis pendant le décompte led sa serra détecter comme faux
 *			départs, à la fin du décompte a vous d'appuiez le plus vite 
 *			possible, le vinqueur serra afficher ainsi que son score, si le 
 *			best score a état battue cela serra afficher aussi, après cela 
 *			les 2 score s'affiche pour que les joueurs les regarder pendant
 *			3 seconds, après a vous de svoir si vous voulez rejouer ou pas.
 *			Si vous rejouer pas faite une longue pulse se qui vas directment   
 *			rammener au debut dans le menu, si vous voulez rejouer courte 
 *			pulse et cela relence une partie entre les 2 même joueurs
 *
 *			dans tout le code il y a un watchdog timer de 8sec qui est 
 *			rénitialiser tout au long du code pour éviter un reset improbable.
 *			dans les moment ou il faut une interctions du joueur sauf dans le 
 *			menu le watchdog timer reset au bout de 8 secondes.
 *
 *			une courte pulse < 500ms
 *			une longue pulse > 500ms
 */ 
#include "global.h"

U_TWI_MSG led;

/*variable initialisations*/
uint16_t tON = 0;
uint16_t tON2 = 0;
uint8_t names = 0;
uint16_t scoreP1 = 0x0000;
uint16_t scoreP2 = 0x0000;
uint16_t score = 0;
uint16_t bestScoreReflex = 0;
uint8_t addrEEPROM = 0x00;
uint8_t winner = 0;

uint8_t ledStart[] = {252,240,192,0};
uint8_t ledON[] = {255,126,60,24,0,24,60,126};

char *p1 = "";
char *p2 = "";
char i2cMsg[] = "";	
char *listePlayers[] = {"BOA", "SCJ", "DUI", "PED",
						"STT", "MOR", "JAB", "LEM",
						"NOP", "GOP", "DUJ", "BUN",
						"BLN","BUE", "TOG", " "," "};

/*structure de tous les flag*/
typedef struct
{
	uint16_t flagP1ShortPulse:1;
	uint16_t flagP1LongPulse:1;
	uint16_t flagP2ShortPulse:1;
	uint16_t flagP2LongPulse:1;
	uint16_t flagStartGame:1;
	uint16_t restart:1;
	uint16_t flagUpdateDisplay:1;
	uint16_t p1_Select:1;
	uint16_t p2_Select:1;
	uint16_t p1_Ready:1;
	uint16_t p2_Ready:1;
	uint16_t wrongStart:1;
	uint16_t reset_score:1;
}S_flag;

/*machine d'état global*/
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


/*machine d'état séléction des jeux*/
typedef enum
{
	SELECT_GAME_1,
	SELECT_GAME_2,
	SELECT_GAME_3,
	SELECT_GAME_4
}E_GAME_STATE;


/*machine d'état sélection des noms*/
typedef enum
{
	SELECT_PLAYER1,
	SELECT_PLAYER2
}E_SELECT_PLAYER;

/*machine d'état du jeux reflex*/
typedef enum
{
	BEST_SCORE,
	SCORE_PLAYERS_START,
	PLAY,
	WRONG_START
}E_REFLEX;

/*machine d'état pour le timer1*/
typedef enum
{
	TIMER_FOR_PULSE,
	TIMER_FOR_SCORE,
}E_TIMER;

/*initialisations des variable des machines d'états*/
E_SM_GLOBAL global = STATE_SELECT_GAME;
E_SM_GLOBAL games_selected = STATE_GAME1_PLAYING;
E_GAME_STATE StateGameSelect = SELECT_GAME_1;
E_SELECT_PLAYER statePlayerSelect = SELECT_PLAYER1;
E_REFLEX duellator = BEST_SCORE;
E_TIMER timer = TIMER_FOR_PULSE;
S_flag flag;


/*interuption pour la prise d'une longue ou court pulse
  pour les 2 jouerus*/
ISR (PCINT2_vect)
{
	/*attend flanc descandant joueur 1*/
	if (PIND & ~(1 << PIND4))
	{
		/*par rapport au tON short ou long pulse*/
		if (tON <= 500 && tON > 0)
		{
			flag.flagP1ShortPulse = 1;
		}
		else if (tON > 500)
		{
			flag.flagP1LongPulse = 1;
		}
		
		tON = 0;
		TCCR1B &= ~(1 << CS10);
		TCNT1 = 0;
		flag.p1_Select = 0;
	}
	
	/*attend flanc descandant joueur 2*/
	if (PIND & ~(1 << PIND5))
	{
		/*par rapport au tON2 short ou long pulse*/
		if (tON2 <= 500 && tON2 > 0)
		{
			flag.flagP2ShortPulse = 1;
		}
		else if (tON2 > 500)
		{
			flag.flagP2LongPulse = 1;
		}
		
		tON2 = 0;
		TCCR1B &= ~(1 << CS10);
		TCNT1 = 0;
		flag.p2_Select = 0;
	}
	
	/*flanc montant pour activer le timer */
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

/*comparaison toute les 1ms pour les pulse et le score
  du jeux*/
ISR (TIMER1_COMPA_vect)
{
	switch (timer)
	{
		case TIMER_FOR_PULSE:
		
			if (flag.p1_Select != 0)
			{
				tON++;
			}
			if (flag.p2_Select != 0)
			{
				tON2++;
			}
			
		break;
		
		case TIMER_FOR_SCORE:
			score++;
		break;
		
		default:
		break;
	}	
}

/*donne une random delay par rapport au score du joueur 1*/
uint16_t giveArandomDelay(uint16_t scoreRDM)
{
	uint16_t delay = 0;
	uint16_t rdmDelay[32] = {450,156,321,241,8,69,210,478,21,278,632,285,
							 102,368,752,985,52,39,750,621,52,765,50,988,
							 89,200,300,389,486,599,185,233};
	 delay = scoreRDM % 32;
	 
	 return rdmDelay[delay];
}

/*création fountion delay*/
void delay(uint16_t milliseconds)
{
	uint16_t iterations = (F_CPU / 10000) * milliseconds;
	
	for (uint16_t i = 0; i < iterations; i++) 
	{
		asm volatile("nop");
	}
}

/*init des interuption externe */
void init_interPCINT(void)
{
	PCICR |= (1 << PCIE2);
}

/*init du timer*/
void init_Timer1(void)
{
	TCCR1A |= 0x00;
	TCCR1B |= (1 << WGM12);
	TIMSK1 |= (1 << OCIE1A);
	OCR1A = 7999;
}

/*init du watch dog timer*/
void init_WDT(void)
{
	wdt_reset();
	WDTCSR |= (1 << WDCE) | (1 << WDE);
	WDTCSR = ((1 << WDIE) | (1 << WDE) | (1 << WDP3) | (1 << WDP0));
}

/*animations a la mise sous tensions de l'appareils ou rest du WDT*/
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
	
	/*addresse du LCD et le mode ecriture*/
	led.S_MsgSplit.SlaveAdress = SLAVEADDR_PCF8574A;
	led.S_MsgSplit.ReadWriteBit = TWI_WRITE;
	
	bliking_Start();
	
	flag.flagUpdateDisplay = 1;
	
	/*a maintenire pour reset le best score*/
	if ((PIND & (1 << PIND4)) && (PIND & (1 << PIND5)))
	{
		flag.reset_score = 1;
		LCD_Clear();
		LCD_PutString("RESET SCORE!!");
		
		bestScoreReflex = 666;
		eeprom_write_byte((uint8_t *)0, (bestScoreReflex >> 8) & 0xFF);
		eeprom_write_byte((uint8_t *)1,  bestScoreReflex & 0xFF);
		
		_delay_ms(2000);
		
		while((PIND & (1 << PIND4)) || (PIND & (1 << PIND5)));
	}
	
    /* Replace with your application code */
    while (1) 
    {
		switch (global)
		{
			/*case du choix du jeux*/
			case STATE_SELECT_GAME:
			
				PCMSK2 = (1 << PCINT20);
				
				/*court pulse pour parcourir la liste des jeux*/
				if (flag.flagP1ShortPulse != 0)
				{
					flag.flagP1ShortPulse = 0;
					flag.flagP2ShortPulse = 0;
					flag.flagUpdateDisplay = 0x01; 
					 
					 /*choix du jeux*/
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
				
				/*la longue pulse pour séléectioner le jeux*/
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
						
						/*court pulse faire dérouler les names*/
						/*chaque pulse le variable name s'incrémente
						  pour changer de name dans la liste des noms*/
						if (flag.flagP1ShortPulse != 0)
						{
							wdt_reset();
							flag.flagP1ShortPulse = 0;
							flag.flagUpdateDisplay = 0x01;
							
							names += 1;
							if (names == 15)
							{
								names = 0;
							}
						}
						
						/*valide le name*/
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
						
						/*court pulse faire dérouler les names*/
						/*chaque pulse le variable name s'incrémente
						  pour changer de name dans la liste des noms*/
						if (flag.flagP2ShortPulse)
						{
							wdt_reset();
							flag.flagP2ShortPulse = 0;
							flag.flagUpdateDisplay = 0x01;
							
							names += 1;
							if (names == 15)
							{
								names = 0;
							}
						}
						
						/*valide le name*/
						if (flag.flagP2LongPulse != 0)
						{
							wdt_reset();
							flag.flagP2LongPulse = 0;
							
							/*empeche de séléctionner le même nom*/
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
				
				/*short pulse pour dire prêt*/
				if (flag.flagP1ShortPulse != 0 && flag.p1_Select == 0)
				{
					wdt_reset();
					flag.flagUpdateDisplay = 1;
					flag.p1_Select = 1;
				}
				
				if (flag.flagP2ShortPulse != 0 && flag.p2_Select == 0)
				{
					wdt_reset();
					flag.flagUpdateDisplay = 1;
					flag.p2_Select = 1;
				}
				
				/*active que après le premiers round*/
				if (flag.restart != 0)
				{
					/*longue pulse si un joueur veux pas rejouer*/
					/*retourne à l'état d'orgine des variables pour les
					  machines d'états*/
					if (flag.flagP1LongPulse != 0 || flag.flagP2LongPulse != 0)
					{
						flag.flagUpdateDisplay = 1;
						flag.flagP1LongPulse = 0;
						flag.flagP2LongPulse = 0;
						flag.flagP2ShortPulse = 0;
						flag.flagP1ShortPulse = 0;
						flag.p1_Select = 0;
						flag.p2_Select = 0;
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
							
							/*décompt led + faux départs pris en compte*/
							for (uint8_t i = 0; i < 4; i++)
							{
								wdt_reset();
								
								/*valeurs a envoyer en I2C pour le PCF*/
								led.S_MsgSplit.Message[0] = ledStart[i];
								TWIMaster_SendMsg(led.MsgBuffer , 2);
								while(TWI_isBusy());
								
								/**/
								for (uint8_t y = 0; y < 251; y++)
								{
									/*détecte un faux départ*/
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
							
							/*si pas de faux départs*/
							if (flag.wrongStart != 1)
							{	
								/*donne le randome delay*/
								delay(giveArandomDelay(scoreP1));
								wdt_reset();
								
								/*éteint les led pour indiquer le départs*/   
								led.S_MsgSplit.Message[0] = 255;
								TWIMaster_SendMsg(led.MsgBuffer , 2);
								while(TWI_isBusy());
								
								score = 0;
								
								PORTD = 0x00;
								
								/*états de la partie du timer pour l'inc du 
								score*/
								timer = TIMER_FOR_SCORE;
								
								/*passe a l'état pour jouer*/
								duellator = PLAY;
								
								/*active le timer*/
								TCCR1B |= (1 << CS10);
							}
						}
						
					break;
				
					case PLAY:
					
						/*enregistre le score joueur 1 flanc montant*/
						if ((PIND & (1 << PIND4)) && (flag.p1_Select != 1))
						{
							scoreP1 = score;
							flag.p1_Select = 1;
						}
						
						/*enregistre le score joueur 2 flanc montant*/
						if ((PIND & (1 << PIND5)) && (flag.p2_Select != 1))
						{
							scoreP2 = score;
							flag.p2_Select  = 1;
						}
						
						/*si les deux joueur on jouer*/
						if ((flag.p1_Select != 0) && (flag.p2_Select != 0))
						{
							flag.p1_Select = 0;
							flag.p2_Select = 0;
							score = 0;
							TCCR1B &= ~(1 << CS10);
							TCNT1 = 0;
							
							/*boucle qui assure l'attente du rélachements des
							  bouton pour continuer*/
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
			
				/*scorP1 plus petit P1 gagne*/
				if (scoreP1 < scoreP2)
				{
					winner = 1;
				}
				/*scorP2 plus petit P2 gagne*/
				else if (scoreP2 < scoreP1)
				{
					winner = 2;
				}
				/*sinon même score égaliter*/
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
		
/*********************************SHWO DISPLAY*******************************/

		if(flag.flagUpdateDisplay != 0)
		{
			flag.flagUpdateDisplay = 0;
			
			switch(global)
			{
				case STATE_SELECT_GAME:
				
					/*affichage du menu*/
					LCD_Clear();
					LCD_setCursor(0,0);
					LCD_PutString("------MENU------");
					LCD_setCursor(1,0);
					LCD_PutString("GAME: ");
					
					/*liste des jeux*/
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
						/*affichage des noms et sélection*/
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
							/**/
						break;
					}
					
				 break;
				 
				 case STATE_WAIT_READY:
				 
					LCD_Clear();
					LCD_setCursor(0,0);
					
					/*ready pour le premiers round*/
					if (flag.restart == 0)
					{
						LCD_PutString("-----READY?-----");
					}
					/*popropose si les même joueurs veulent rejouer*/
					else
					{
						LCD_PutString("Press to restart");
						flag.flagStartGame = 0;
					}
					
					LCD_setCursor(1,0);
					
					/*si chaquin fait une short pulse mais ok */
					LCD_PutString(p1);
					if (flag.flagP1ShortPulse != 0)
					{
						LCD_PutString(": OK  ");
						flag.p1_Ready = 1;
					}
					else
					{
						LCD_PutString(": NO  ");
					}
					
					LCD_PutString(p2);
					if (flag.flagP2ShortPulse != 0)
					{
						LCD_PutString(": OK");
						flag.p2_Ready = 1;
					}
					else
					{
						LCD_PutString(": NO");
					}
					
					/*short puls si ui pour jouer ou rejouer des 2 joueurs*/
					if (flag.flagP1ShortPulse != 0 && flag.flagP2ShortPulse != 0)
					{	
						if ((flag.p1_Ready == 1) && (flag.p2_Ready == 1))
						{
							global = games_selected;
							PCMSK2 &= ~((1 << PCINT20) | (1 << PCINT21));
							flag.flagP2ShortPulse = 0;
							flag.flagP1ShortPulse = 0;
							flag.flagUpdateDisplay = 1;
							flag.restart = 0;
							flag.p1_Ready = 0;
							flag.p2_Ready = 0;
							
							/**/
							switch (games_selected)
							{
								case STATE_GAME1_PLAYING:
								
								flag.flagP2LongPulse = 0;
								flag.flagP1LongPulse = 0;
								flag.flagStartGame = 0;
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
					}
					
				 break;
				 
				 case STATE_GAME1_PLAYING:
					switch (duellator)
					{
						case BEST_SCORE:
						
							/*affiche le best score en début de round*/
							LCD_Clear();
							LCD_setCursor(0,0);
							LCD_PutString("-THE GAME START-");
							LCD_setCursor(1,0);
							LCD_PutString("BEST SCORE: ");
							
							/*prend le score sauvgarder en EEPROM*/
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
						
							/*affichage des enciens score en début de partie*/
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
							
							/*affiche quelle joueur a fait un WRONG START*/
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
							
							/*clignotement montrant le wrong start sur les
							 leds*/
							for (uint8_t b = 0; b < 13; b++)
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
							flag.restart = 1;

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
					
					/*winner 3 position :
						  winner = 0 -> égalité
						  winner = 1 -> Joueur 1 vinqueur
						  winner = 2 -> Joueur 2 vinqueur*/
					if (winner != 0)
					{
						LCD_PutString("-----WINNER-----");
					}
					else
					{
						LCD_PutString("-----EGALITE----");
					}
					LCD_setCursor(1,0);
					
					/*clignotement pour le vinqueur*/	
					if (winner == 1)
					{
						LCD_PutString(p1);
						LCD_PutString(": ");
						sprintf(i2cMsg, "%d", scoreP1);
						LCD_PutString(i2cMsg);
						
						for (uint8_t a = 0; a < 20; a++)
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
						
						for (uint8_t a = 0; a < 20; a++)
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
				 
					/*si on a décider de reset le score*/
					/*bestscore = 666*/
					/*sinon bestscore prend se qui a dans l'EEPROM*/
					/*sert pour la condition suivante*/
					bestScoreReflex = (eeprom_read_byte((uint8_t *)0) << 8);
					bestScoreReflex = bestScoreReflex  + eeprom_read_byte((uint8_t *)1);
					
					/*si le bestscore et plus grand que le score d'un des 2 joueurs*/
					if ((scoreP1 < bestScoreReflex) || (scoreP2 < bestScoreReflex))
					{
						flag.reset_score = 0;
						eeprom_write_byte(0,0);
						/*si p1 a un plus petit score que p2*/
						if (scoreP1 < scoreP2)
						{
							eeprom_write_byte((uint8_t *)0, (scoreP1 >> 8) & 0xFF);
							eeprom_write_byte((uint8_t *)1,  scoreP1 & 0xFF);
							flag.p1_Select = 1;
						}
						/*si p2 a un plus petit score que p1*/
						else if (scoreP2 < scoreP1)
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
						
						/*affiche le quelle a battue le Bestscore sur le LCD*/
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
					
					flag.flagUpdateDisplay = 1;
					global = STATE_SHOW_SCORE;
		
				 break;
				 
				 case STATE_SHOW_SCORE:
						
					/*affiche les 2 scores*/
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
					
					/*retourne dans wait ready pour demander de restart 
					ou pas*/
					global = STATE_WAIT_READY;
					PCMSK2 |= ((1 << PCINT20) | (1 << PCINT21));
					PCICR |= (1 << PCIE2);
					flag.flagUpdateDisplay = 1;
					flag.restart = 1;
					
				 break;
				 
				 default:
				 break;
			 }
		 }
	}
}