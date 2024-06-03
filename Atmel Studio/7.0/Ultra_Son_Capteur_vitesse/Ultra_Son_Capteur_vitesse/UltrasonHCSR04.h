/*
 * UltrasonHCSR04.h
 *
 * Created: 6/29/2021 8:37:44 AM
 *  Author: AppMan
 */ 

#ifndef ULTRASONHCSR04_H_
#define ULTRASONHCSR04_H_

// Si F_CPU non définie --> règle à 8MHz
#ifndef F_CPU
	#define F_CPU 16000000UL		// Fréquence d'horloge (!! Fusibles à régler)
#endif

/****************** Includes ****************************/
#include <avr/interrupt.h>		// Pour les vecteurs d'interruption
#include <avr/io.h>				// Pour les entrées-sorties
#include <stdint.h>				// Pour les types standards (uint8_t, int8_t, etc)
#include <util/delay.h>


/****************** Defines  ****************************/
#define CS_T0T1_MASK	0x07	// Masque de sélection de la source d'horloge
#define WGM_11_10_MASK	0x03	// Masque de sélection des bits WGM10 et WGM11 (Timer 1)
#define WGM_13_12_MASK	0x18	// Masque de sélection des bits WGM13 et WGM12 (Timer 1)
#define COM_A_B_MASK	0xF0	// Masque de sélection des bits COM1An et COM1Bn (T1 + T0)

#define T1_MODE_PWM_OCR		0x0F

#define MEAS_BUFFER_LEN 10	// Length of buffer

/****************** Types  ****************************/
typedef enum
{
	CAPTURE_WAIT_START,
	CAPTURE_STARTED,
	CAPTURE_STOPED,
	CAPTURE_ERROR
}E_CAPTURE_STATUS;

/****************** prototypes  ***********************/

void UltraSon_GPIO_INIT(void);
void Init_Timer(void);
void Timer_Start(void);
void Timer_Stop(void);
void averageAddTable(uint16_t NewValue);
uint16_t getAverageDistance(uint8_t nSamples);


#endif /* ULTRASONHCSR04_H_ */