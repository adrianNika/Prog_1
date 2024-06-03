#include <avr/io.h>

#define WRITE_2_POT		0b00011111	//	(XX)nop(01)écriture(XX)nop(11)2pot
#define	WRITE_GAUCHE 	0b00011110	//	écriture gauche
#define	WRITE_DROITE 	0b00011101	//	écriture droite

#define DDR_SPI		DDRB	//	direction du port SPI
#define DD_MOSI		PIN5	//	SPI broche MOSI
#define DD_MISO		PIN6	//	SPI broche MISO
#define DD_SCK		PIN7	//	SPI broche Clock
#define DD_SS		PIN4	//	SPI broche Slave


void SPI_MasterInit(void)
{

	DDR_SPI |= (1<<DD_MOSI) | (1<<DD_SCK) | (1<<DD_SS);	// SS,MOSI,SCK en sortie
	DDR_SPI &= ~(1<<DD_MISO);	// MISO en entrée

	SPCR |= (1<<SPE) | (1<<MSTR) ;//| (1<<SPR1) | (1<<SPR0);	// Valide SPI, Maître débit à fck/128
	//SPSR |= (1<<SPI2X);
	//SPCR |= (1<<CPOL) |(1<<CPHA);		// SPI mode 0
}

void SPI_MasterTransmit(long data)
{
	SPDR = data;				// Mise des données pour l'envoi SPI
	while(!(SPSR & (1<<SPIF)));	// Attente de la fin de l'envoi
}

void SPI_MasterStop()
{
	int dummy = SPDR;			// Arret de l'interruption
								// Par lecture du registre
	SPCR &= ~(1<<SPE);			// Arret du SPI
	//return recept;			// Retourne la lecture SPI
}

void SetPot(int Data,int Command)
{		

		SPI_MasterInit();					//	Initialisation du SPI

		PORTB &= ~(1<<DD_SS);				//	Activation de l'esclave

			
		SPI_MasterTransmit(Command);		//	Transmission SPI


		SPI_MasterTransmit(Data);			//	Des 2x8 bits pour MCP42010


		PORTB |= (1<<DD_SS);				//	Désactivation de l'esclave


		SPI_MasterStop();					//	Arret du SPI

}
