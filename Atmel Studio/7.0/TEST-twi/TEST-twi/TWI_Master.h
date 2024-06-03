/*
 * TWI_Master.h
 *
 * Auteur :		David Perrin
 * Date :		19.06.2020
 * Version :	V1
 *
 * Description : 
 *		Fichier de définition de la librairie TWI_Master.c
 */ 
#ifndef TWI_MASTER_H
#define TWI_MASTER_H

// Include
#include <stdint-gcc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

// Définitions 

//  *********************  Protocole I2C   ************************************
#define TWI_RW_BIT		0x01	// Bit de "LECTURE/ECRITURE" (Opération SLA + R/W)

#define TWI_WRITE		0x00	// Lors d'une écriture, le bit doit être à "0"
#define TWI_READ		0x01	// Lors d'une lecture, le bit doit être à "1"	

#define TWI_BUFFER_LEN	0x03	// Défini la taille du buffer de transmission

//  *********************  Vitesse SCL   **************************************
// CHOISIR ICI la vitesse de l'I2C (Vitesse standard --> 100kHz / 400kHz) 
// En général --> PRESCALER = 0 
// 
// Selon la vitesse du bus I2C et de F_CPU, il est nécessaire de calculer 
// une autre pair "PRESCALER"/"TWBR", selon la relation suivante (DS p.222) 
//
//					(F_CPU / SCL_FREQU) - 16		F_CPU = Fréquence clock [Hz]
//		    TWBR = ---------------------------		F_SCL = Fréquence I2C [Hz]
//						 2 * Prescaler				Prescaler = prédiviseur [-] 
//
//	Pour un prescaler de  1 --> TSWR(bit 1) = 0 et TWSR(bit 0) = 0
//	Pour un prescaler de  4 --> TSWR(bit 1) = 0 et TWSR(bit 0) = 1
//  Pour un prescaler de 16 --> TSWR(bit 1) = 1 et TWSR(bit 0) = 0
//	Pour un prescaler de 64 --> TSWR(bit 1) = 1 et TWSR(bit 0) = 1
// Si valeur de TWBR > 255, alors augmenter PRESCALER (1,4,16,64) (De base à 1).
//
// Exemple: 
//	(F_CPU =  8MHz, F_SCL = 100kHz) --> Prescaller = 0 et TWBR = 0x20
//  (F_CPU =  8MHz, F_SCL = 400kHz) --> Prescaller = 0 et TWBR = 0x02
//	(F_CPU = 16MHz, F_SCL = 100kHz) --> Prescaller = 0 et TWBR = 0x48
//  (F_CPU = 16MHz, F_SCL = 400kHz) --> Prescaller = 0 et TWBR = 0x0C
//	(F_CPU = 20MHz, F_SCL = 100kHz) --> Prescaller = 0 et TWBR = 0x48
//  (F_CPU = 20MHz, F_SCL = 400kHz) --> Prescaller = 0 et TWBR = 0x0C 


// Réglage pour F_SCL = 100kHz avec F_CPU = 8 MHz
#define TWI_PRESCAL		0x00		// Valeur dans "TWSR bit 1 et b0" 
#define MASK_TWI_PRES	0x03		// Masque de sélection du prescaler

#define TWI_TWBR		0x18		// Registre de vitesse de transfert (BitRate)


// *********************  Status TWSR   ***************************************
// Ces codes correspondent à l'état du registre TWSR, ils sont valables uniquement
// lorsque le module TWI (Hardware) a terminé une étape. Le module génère alors
// une Interruption et met à jour le status de TWSR. 

#define TWI_START				0x08	// Condition START envoyée 
#define TWI_REP_START			0x10	// Condition REPEAT START envoyée 

#define TWI_WR_ADR_ACK			0x18	// SLA + W (écriture) envoyée, ACK reçu
#define TWI_WR_ADR_NACK			0x20	// SLA + W (écriture) envoyée, NACK reçu 
#define TWI_WR_DATA_ACK			0x28	// Data (écriture) envoyée, ACK reçu
#define TWI_WR_DATA_NACK		0x30	// Data (écriture) envoyée, NACK reçu

#define TWI_RD_ADR_ACK			0x40	// SLA + R (lecture) envoyée, ACK reçu
#define TWI_RD_ADR_NACK			0x48	// SLA + R (lecture) envoyée, NACK reçu
#define TWI_RD_DATA_ACK			0x50	// Data (lecture) envoyée, ACK reçu
#define TWI_RD_DATA_NACK		0x58	// Data (lecture) envoyée, NACK reçu 

#define TWI_NO_STATUS			0xF8	// Aucun status correct
#define TWI_ERROR_BUS			0x00	// Erreur sur le bus (opération illégale)

/* Types
***********************  TWI_STATUS  ***************************************
* Il s'agit d'un type qui permettra d'avoir une variable donnant
* l'information de l'état du périphérique 
*/
typedef union
{
	struct  
	{
		uint8_t		TransmitOk:1;
		uint8_t		unused:7;
	}S_StatusBit;
	uint8_t StatusByte;
}U_TWI_Status;

/* U_TWI_MSG
* Structure permettant de faciliter la préparation des données (adresse décallée) 
*/

typedef union
{
	struct  
	{
		uint8_t ReadWriteBit:1;				// Bit Field --> Décallage auto de l'adresse
		uint8_t SlaveAdress:7;				
		uint8_t Message[TWI_BUFFER_LEN-1];	
	}S_MsgSplit;
	
	uint8_t MsgBuffer[TWI_BUFFER_LEN];
}U_TWI_MSG;

// Prototypes FCT
void TWIMaster_Init(void);
void TWIMaster_SendMsg(uint8_t *Msg, uint8_t MsgSize);
U_TWI_Status TWI_GetData(uint8_t *Msg, uint8_t MsgLength);
U_TWI_Status TWI_GetTransmitStatus ();
uint8_t TWI_isBusy(void);


#endif /* TWI_MASTER_H */