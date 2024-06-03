//Routine pour  utiliser le hard du ATMEGA8
//jean-yves dupertuis
//20.08.2009
//modif : ajout routine gestion LCD EA T123W 02.07.2014
//		: ajout d'une fonction Clear_Lcd_I2C 09.07.2014 crée par JOB
//modif : ajout routine pour le MCP 23016 30.04.2015 DUJ
//vers:0.3

#include <avr/io.h>
#include <util/delay.h>

int compteur, ad, add_8, rw,data_8, pcf;

void ERROR(void)
{
_delay_ms(100);
//--------init clavier---------
Ecriture_I2C(0b01000000,0xFF);
_delay_ms(100);
Ecriture_I2C(0b01000010,0x00);
_delay_ms(100);
}

void Initialisation_I2C(void)
{
	TWBR=0x30;
}

int adresse_I2C(int pcf,int ad,int rw)
{
	int adresse;
	adresse= pcf | ad | rw ;
	return adresse;
}

void Start_I2C(void)
{
	TWCR= (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); //envoi start
	while (!(TWCR & (1<<TWINT)))   //attend le flag d'envoi du start 
	{
	 _delay_us(20);
	}
}

void Envoi_I2C(int add_data)
{
	TWDR= add_data;
	TWCR= (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)))
	{
	 _delay_us(20);
	}
}

void Stop_I2C(void)
{
	TWCR= (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

void Ecriture_I2C(int add,int data)
{
	int Adresse, Data;
	Adresse= add ;
	Data = data;
	
	Start_I2C();
	Envoi_I2C(Adresse);
	
	Envoi_I2C(Data);
	Stop_I2C;
	Adresse= 0 ;
	Data = 0;
	Stop_I2C;
	_delay_ms(20);
}

int Lecture_I2C(int add)
{
	int Adresse= add;

	Start_I2C();
	Envoi_I2C(Adresse);

	TWCR= (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)))
	{
	 _delay_us(10);
	}
	return TWDR;
}

// fonction pour la gestion de l ecran EA T123
//
// normalement l adresse du LCD est la suivante. (verification !)
//
//add_display= adresse_I2C(0x74,0x00,0);
//
//normalement l adresse du debut de la 1ere ligne est 0x80
//                                     2eme ligne est 0xA0
//                                     3eme ligne est 0xC0 
//
//pour tester voici une table :
//
//char titre [12]= {0xd4,0xe9,0xe3,0xa0, 0xd4, 0xe1, 0xe3, 0xa0, 0xd4, 0xef, 0x85,0xa0};
//

void Initialisation_Lcd_I2C(void)
{
	int add_display= adresse_I2C(0x74,0x00,0);

	Start_I2C();

	Envoi_I2C(add_display);

	Envoi_I2C(0x00);          //Fonction SET

	Envoi_I2C(0x06);          //increment droite display fixe

	Envoi_I2C(0x0E);          //Diplay on

	Envoi_I2C(0x2E);          //3lignes

	
	Position_texte_Lcd(add_display,0xC4);
}

void Envoi_Chaine_I2C(int add_display, char *table)    //reception de l'adresse du debut de la table
{
	int i;
	Start_I2C();
	Envoi_I2C(add_display);
	Envoi_I2C(0x40);          //byte de controle d ecriture
	
	i=0;
	while (i<12)
	{
		Envoi_I2C(*table);    //envoi de la valeur pointee par l'adresse
		i=i+1;
		table= table + 1;     //incrementation de l'adresse

	}
}

void Position_texte_Lcd(int add_display,int position)
{
	Start_I2C();

	Envoi_I2C(add_display);

	Envoi_I2C(0x00);          //Fonction SET

	Envoi_I2C(position);          //Diplay 3e ligne(80 ligne1, A0 ligne2, C0 ligne3
}

void Clear_Lcd_I2C(int add_display)//efface l'écran LCD (caractère vide)
{
	char efface [12] = {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};

	Position_texte_Lcd(add_display, 0x80);//texte sur la ligne 1
	Envoi_Chaine_I2C(add_display,efface);//efface la ligne 1
	Position_texte_Lcd(add_display,0xA0);//texte sur la ligne 2
	Envoi_Chaine_I2C(add_display,efface);//efface la ligne 2
	Position_texte_Lcd(add_display,0xC0);//texte sur la ligne 3
	Envoi_Chaine_I2C(add_display,efface);//efface la ligne 3
}

/* fonction pour MCP23016, le style d'adressage se trouve au même endroit que le PCF 8574
 mais il permet de doubler le nombre d'entrées - sorties
*/
void Ecriture_I2C_MCP(int add,int registre,int data)
{
	//fonction de base permettant de tout faire
	int Adresse, Data;
	Adresse= add ;
	Data = data;
	
	Start_I2C();
	Envoi_I2C(Adresse);
	Envoi_I2C(registre);	
	Envoi_I2C(Data);
	Stop_I2C;
	Adresse= 0 ;
	Data = 0;
	Stop_I2C;
	_delay_ms(20);
}

void Ecriture_I2C_MCP16(int add,int registre,int io1,int io2)
{
	Start_I2C();
	Envoi_I2C(add);
	Envoi_I2C(registre);	
	Envoi_I2C(io1);
	Envoi_I2C(io2);
	Stop_I2C;
	add= 0 ;
	
	Stop_I2C;
	_delay_ms(20);
}
