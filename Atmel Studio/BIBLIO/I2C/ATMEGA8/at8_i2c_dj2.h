//Routine pour  utiliser le hard du ATMEGA8
//jean-yves dupertuis
//20.08.2009
//modif : ajout routine gestion LCD EA T123W 02.07.2014
//		: ajout d'une fonction Clear_Lcd_I2C 09.07.2014 crée par JOB
//modif : ajout routine pour le MCP 23016 30.04.2015 DUJ
//vers:0.3

#ifndef AT8_I2C_DJ
#define AT8_I2C_DJ

#include <avr/io.h>
#include <util/delay.h>


void ERROR(void);
void Initialisation_I2C(void);
int adresse_I2C(int pcf,int ad,int rw);
void Start_I2C(void);
void Envoi_I2C(int add_data);
void Stop_I2C(void);
void Ecriture_I2C(int add,int data);
int Lecture_I2C();


// fonction pour la gestion de l ecran EA T123
void Initialisation_Lcd_I2C(void);
void Envoi_Chaine_I2C(char *table);
void Position_texte_Lcd(position);
void Clear_Lcd_I2C(void);


// MCP23016

void Ecriture_I2C_MCP(int add,int registre,int data);
void Ecriture_I2C_MCP16(int add,int registre,int io1,int io2);

#endif
