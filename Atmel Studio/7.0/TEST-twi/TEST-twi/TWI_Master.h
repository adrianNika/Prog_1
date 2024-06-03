/*
 * TWI_Master.h
 *
 * Auteur :		David Perrin
 * Date :		19.06.2020
 * Version :	V1
 *
 * Description : 
 *		Fichier de d�finition de la librairie TWI_Master.c
 */ 
#ifndef TWI_MASTER_H
#define TWI_MASTER_H

// Include
#include <stdint-gcc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

// D�finitions 

//  *********************  Protocole I2C   ************************************
#define TWI_RW_BIT		0x01	// Bit de "LECTURE/ECRITURE" (Op�ration SLA + R/W)

#define TWI_WRITE		0x00	// Lors d'une �criture, le bit doit �tre � "0"
#define TWI_READ		0x01	// Lors d'une lecture, le bit doit �tre � "1"	

#define TWI_BUFFER_LEN	0x03	// D�fini la taille du buffer de transmission

//  *********************  Vitesse SCL   **************************************
// CHOISIR ICI la vitesse de l'I2C (Vitesse standard --> 100kHz / 400kHz) 
// En g�n�ral --> PRESCALER = 0 
// 
// Selon la vitesse du bus I2C et de F_CPU, il est n�cessaire de calculer 
// une autre pair "PRESCALER"/"TWBR", selon la relation suivante (DS p.222) 
//
//					(F_CPU / SCL_FREQU) - 16		F_CPU = Fr�quence clock [Hz]
//		    TWBR = ---------------------------		F_SCL = Fr�quence I2C [Hz]
//						 2 * Prescaler				Prescaler = pr�diviseur [-] 
//
//	Pour un prescaler de  1 --> TSWR(bit 1) = 0 et TWSR(bit 0) = 0
//	Pour un prescaler de  4 --> TSWR(bit 1) = 0 et TWSR(bit 0) = 1
//  Pour un prescaler de 16 --> TSWR(bit 1) = 1 et TWSR(bit 0) = 0
//	Pour un prescaler de 64 --> TSWR(bit 1) = 1 et TWSR(bit 0) = 1
// Si valeur de TWBR > 255, alors augmenter PRESCALER (1,4,16,64) (De base � 1).
//
// Exemple: 
//	(F_CPU =  8MHz, F_SCL = 100kHz) --> Prescaller = 0 et TWBR = 0x20
//  (F_CPU =  8MHz, F_SCL = 400kHz) --> Prescaller = 0 et TWBR = 0x02
//	(F_CPU = 16MHz, F_SCL = 100kHz) --> Prescaller = 0 et TWBR = 0x48
//  (F_CPU = 16MHz, F_SCL = 400kHz) --> Prescaller = 0 et TWBR = 0x0C
//	(F_CPU = 20MHz, F_SCL = 100kHz) --> Prescaller = 0 et TWBR = 0x48
//  (F_CPU = 20MHz, F_SCL = 400kHz) --> Prescaller = 0 et TWBR = 0x0C 


// R�glage pour F_SCL = 100kHz avec F_CPU = 8 MHz
#define TWI_PRESCAL		0x00		// Valeur dans "TWSR bit 1 et b0" 
#define MASK_TWI_PRES	0x03		// Masque de s�lection du prescaler

#define TWI_TWBR		0x18		// Registre de vitesse de transfert (BitRate)


// *********************  Status TWSR   ***************************************
// Ces codes correspondent � l'�tat du registre TWSR, ils sont valables uniquement
// lorsque le module TWI (Hardware) a termin� une �tape. Le module g�n�re alors
// une Interruption et met � jour le status de TWSR. 

#define TWI_START				0x08	// Condition START envoy�e 
#define TWI_REP_START			0x10	// Condition REPEAT START envoy�e 

#define TWI_WR_ADR_ACK			0x18	// SLA + W (�criture) envoy�e, ACK re�u
#define TWI_WR_ADR_NACK			0x20	// SLA + W (�criture) envoy�e, NACK re�u 
#define TWI_WR_DATA_ACK			0x28	// Data (�criture) envoy�e, ACK re�u
#define TWI_WR_DATA_NACK		0x30	// Data (�criture) envoy�e, NACK re�u

#define TWI_RD_ADR_ACK			0x40	// SLA + R (lecture) envoy�e, ACK re�u
#define TWI_RD_ADR_NACK			0x48	// SLA + R (lecture) envoy�e, NACK re�u
#define TWI_RD_DATA_ACK			0x50	// Data (lecture) envoy�e, ACK re�u
#define TWI_RD_DATA_NACK		0x58	// Data (lecture) envoy�e, NACK re�u 

#define TWI_NO_STATUS			0xF8	// Aucun status correct
#define TWI_ERROR_BUS			0x00	// Erreur sur le bus (op�ration ill�gale)

/* Types
***********************  TWI_STATUS  ***************************************
* Il s'agit d'un type qui permettra d'avoir une variable donnant
* l'information de l'�tat du p�riph�rique 
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
* Structure permettant de faciliter la pr�paration des donn�es (adresse d�call�e) 
*/

typedef union
{
	struct  
	{
		uint8_t ReadWriteBit:1;				// Bit Field --> D�callage auto de l'adresse
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