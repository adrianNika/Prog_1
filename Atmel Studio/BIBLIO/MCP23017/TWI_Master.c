/*
 * TWI_Master.c
 *
 * Auteur :		David Perrin
 * Date :		19.06.2020
 * Version :	V1
 *
 * Description : 
 *		Librairie permettant l'utilisation du p�riph�rique TWI
 *		de l'ATmega88/88PA, ATmega328/328PA
 *		La communication a la particularit� d'�tre "non bloquante" et
 *		bas�e sur une machine d'�tat appel�e sur interruption (TWSR). 
 */ 

#include "TWI_Master.h"

// Variables Globales (pour utilisation de linterruption)
static uint8_t TWI_BufferTxRx[TWI_BUFFER_LEN];  // Buffer de transmission/r�ception 
static uint8_t TWI_MsgSize = 0;					// Taille du message

static U_TWI_Status TWI_StatusTxRx;				// Bit de status de la transmission 



/* TWIMaster_Init()
*  Description: 
* 
* Cette fonction active le module TWI (TWEN). La fr�quence du signal SCL est �galement initialis�
* au traver de cette fonction, il faut cependant choisir dans le fichier de d�finition. 
* !!! Il faut modifier le .h (TWI_PRESCAL +_TWBR) !!!
*
*/

void TWIMaster_Init(void)
{
	// R�glage du prescaler - Pr�sum� � 0
	TWSR &= ~(MASK_TWI_PRES);					// Efface l'ancienne valeur
	TWSR |= (TWI_PRESCAL & MASK_TWI_PRES);		// configure la nouvelle valeur
	
	// Registre de BitRate
	TWBR = TWI_TWBR;							// Voir dans TWI_Master.h
	
	// Registre de donn�e 
	TWDR = 0xFF;								// Valeur initiale (SDA pull-up)
	
	// Activation du TWI
	TWCR = 0;									// Efface l'ancienne config
	TWCR = (1<<TWEN);							// Active le p�rihp�rique TWI
}

/*	TWIMaster_SendMsg
*
*	Cette fonction charge le buffer de transmission avec les donn�es
*	et active l'interruption du p�riph�rique TWI. 
* 
*	Arguments: 
*		- uint8_t *Msg : Contient le message � envoyer
*		- uint8_t MsgSize : Contient la taille du message � envoyer
*	
*	!! Le premier byte du message doit contenir l'adresse + le bit
*	de lecture ou d'�criture !! 
*/

void TWIMaster_SendMsg(uint8_t *Msg, uint8_t MsgSize)		// TODO : ErrorHandle
{	
	// Sauvegarde dans les variables globales
	TWI_MsgSize = MsgSize;
	TWI_BufferTxRx[0] = Msg[0];
	
	// V�rification du mode (Lecture/Ecriture)
	if((Msg[0] & TWI_RW_BIT) == TWI_WRITE)
	{
		// Si en mode lecture, place dans le buffer la suite du
		// message, sinon, ne fait rien (le buffer sera �cras�) 
		for (uint8_t i = 1; i < MsgSize; i++)
		{
			TWI_BufferTxRx[i] = Msg[i];
		}
	}
	
	// R�initialise le statut de transmission
	TWI_StatusTxRx.StatusByte = 0;	
	
	// Active l'interruption pour permettre la transmission du message
	TWCR = (1<<TWEN)|                             // Active le p�riph�rique
		   (1<<TWIE)|(1<<TWINT)|                  // Active l'interruption
		   (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|       // Active la condition de Start uniquement
		   (0<<TWWC);                             // Pas de conflit au d�but (Multi-Master)
	
}

/*	TWI_GetData
*
*	Cette fonction permet de r�cup�rer les donn�es re�ues lors d'une lecture
*	de l'esclave.
*
*	Si les donn�es envoy�es ne sont pas valide, ne modifie pas le buffer de retour
*
*	Argument :
*				- uint8_t *Msg : pointeur du buffer de lecture
*				- uint8_t MsgLength : taille du message (y compris SLA+R [position 0])
*	Retour :
*				- Retourne le status de la transmission (contient TransmitOk)
*/

U_TWI_Status TWI_GetData(uint8_t *Msg, uint8_t MsgLength)
{
	// Si une transmission est en cours, ne fait rien
	if(TWI_isBusy() == 0x00)
	{
		// Si la transmission s'est d�roul�e correctement,
		// �cris dans le buffer
		if (TWI_StatusTxRx.S_StatusBit.TransmitOk)
		{
			for (uint8_t i = 0; i<MsgLength ; i++)
			{
				Msg[i] = TWI_BufferTxRx[i];
			}
		}
	}
	// Retourne le status de la transmission
	return TWI_StatusTxRx;
}

/*	TWI_isBusy
*
*	Cette fonction retourne l'�tat du bit TWIE du registre TWCR.
*
*	Si l'interruption TWI est active, alors une transmission est en cours.
*
*	Arguments:
*		- Aucun
*	Retour:
*		- Etat du bit TWIE
*/

uint8_t TWI_isBusy(void)
{
	return (TWCR & (1<<TWIE));		// TWIE = bit 0, retourne 0x01 si TWIE == 1
}

U_TWI_Status TWI_GetTransmitStatus (void)
{
	return TWI_StatusTxRx;
}


/*	ISR - INTERRUPTION MACHINE d'�tat
*
*	Cette interruption est g�n�r�e � chaque changement d'�tat
*	du p�riph�rique TWI. Ces �tats ne sont valable que si le 
*	bit TWINT est � "1", correspondant � l'interruption. 
*
*	Tant que pointeur du buffer ne corresponds pas � la taille du message, 
*	l'interruption envoie les donn�es qui s'y trouvent. 
*
*/

ISR(TWI_vect)
{
	static uint8_t MsgPtr = 0;		// Pointeur de message (Static = 
								// valeur reste apr�s l'interruption)
								
	switch (TWSR)
	{
		case TWI_START:							// Si une condition de start ou de Repeated start
		case TWI_REP_START:						// a �t� envoy�, remet le pointeur � "0" et charge 
		MsgPtr = 0;								// l'adresse 
		case TWI_WR_ADR_ACK:					// Cette condition s'applique �galement si l'�criture
		case TWI_WR_DATA_ACK:					// a �t� notifi�e par un ACK 
			if (MsgPtr < TWI_MsgSize)			// Si la totalit� n'a pas �t� envoy�e
			{
				TWDR = TWI_BufferTxRx[MsgPtr++];
				TWCR = (1<<TWEN)|                          // Active le p�riph�rique TWI
				(1<<TWIE)|(1<<TWINT)|                      // Clear le Flag TWINT (Continue la transmission)
				(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           //
				(0<<TWWC);                                 //
			}
			else								// Si la totalit� du message a �t� envoy�e								
			{
				TWI_StatusTxRx.S_StatusBit.TransmitOk = 0x01;	// R�gle le bit de status (TX OK) 
				TWCR = (1<<TWEN)|									// Active le p�riph�rique TWI
				(0<<TWIE)|(1<<TWINT)|								// D�sactive les interruption
				(0<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|					// Active la condition de STOP
				(0<<TWWC);											//
			}
		break;
		
    case TWI_RD_DATA_ACK:					// Si un byte a �t� re�u et ACK envoy�
		TWI_BufferTxRx[MsgPtr++] = TWDR;
		
    case TWI_RD_ADR_ACK:					// L'adresse et la notification ont �t� re�u par le SLAVE
    if (MsgPtr < (TWI_MsgSize-1) )          // V�rifie si il s'agit du dernier byte � lire 
    {
		// Si ce n'est pas le cas,
	    TWCR = (1<<TWEN)|                   // Met � Jour le Registre TWCR 
	    (1<<TWIE)|(1<<TWINT)|               //	- clear le FLAG (lecture byte suivant)
	    (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|	//	- Envoie un nack 
	    (0<<TWWC);                          
    }
	else									     
    {
		// Si tout les bytes ont �t�s lus
	    TWCR = (1<<TWEN)|                          // Met � jour le TWCR
	    (1<<TWIE)|(1<<TWINT)|                      // Efface le Flag 
	    (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // Envoie un NACK (Arr�t de lecture)
	    (0<<TWWC);                                 //
    }
    break;
	
    case TWI_RD_DATA_NACK:								// Le dernier byte � �t� re�u (nack envoy�)
		TWI_BufferTxRx[MsgPtr] = TWDR;					// Sauve les donn�es
		TWI_StatusTxRx.S_StatusBit.TransmitOk = 0x01;	// Flag de communication termin�e correctement
		 
		TWCR = (1<<TWEN)|                          // Met � jour le registre de configuration TWI
		(0<<TWIE)|(1<<TWINT)|                      // D�sactive les interruptions et clear le Flag 
		(0<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|           // Envoie la condition de stop 
		(0<<TWWC);                                 //
    break;
	
	// Gestion des erreurs
    case TWI_WR_ADR_NACK:		// Envoi de l'adresse + W, mais slave r�ponds avec NACK
    case TWI_RD_ADR_NACK:		// Envoi de l'adresse + R, mais slave r�ponds avec NACK
    case TWI_WR_DATA_NACK:		// Envoi d'un byte, mais slave r�ponds avec NACK 
    case TWI_ERROR_BUS:			// Error sur le bus (Condition illegale (ex: Start-Stop)
    default:
		//TWI_state = TWSR;				// Sauve le registre de status et
		TWI_StatusTxRx.S_StatusBit.TransmitOk = 0x00;	// Efface le FLag de validation
    
		// Reset TWI Interface
		TWCR = (1<<TWEN)|					// Mise � jour du registre de CONF
		(0<<TWIE)|(0<<TWINT)|				// D�sactive les interruptions
		(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|	// Stoppe la communication en cours
		(0<<TWWC);							//		
	}
}


