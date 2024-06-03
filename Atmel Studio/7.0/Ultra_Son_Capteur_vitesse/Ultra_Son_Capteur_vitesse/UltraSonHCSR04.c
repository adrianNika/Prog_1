/*
 * UltraSonHCSR04.c
 *
 *	Created:	6/29/2021 8:36:58 AM
 *  Author:		David Perrin
 *	Version :	1
 *
 *	Description : 
 *
 *
 *
 */ 

// Include dependencies
#include "UltrasonHCSR04.h"

// Global Variables 
static uint16_t FirstCapture = 0;	// Contains the first snapshot of TCNT1
static uint16_t SecondCapture = 0;	// Contains the second snapshot of TCNT1
static uint16_t countDuration = 0;	// For durantion compute

static uint8_t actualEdge = 1;				// Egde detection for input capture
static uint8_t errorOccuredFlag = 0;		// Flag - error occured
static uint8_t overflowOccured = 0;			// Flag - overflow occured -> =1

// Buffer to save measures
static uint16_t measureBuffer[MEAS_BUFFER_LEN];		// Buffer for measures
static uint32_t SumBuffer = 0 ;						// Summ of buffer
static uint8_t pMeasBuffer = 0;						// Pointer on buffer
static uint8_t flagBufferFull = 0;					// = 0x01 when buffer full 

// Capture Status 
E_CAPTURE_STATUS CaptureStatus = CAPTURE_WAIT_START;

/* Interrupt : Overflow of Timer 1
*
*  Input ARG: none
*  Returns	: none
*  Modify	: overflowOccured	: flag to notify a capture occured
*			  newMeasure		: flag to nitify a new measure is availlable
*			  actualEdge		: 1= rising, 0= falling
*			  CaptureStatus		: Status of capture 
*			  TCCR1B			
*/ 
ISR(TIMER1_OVF_vect)
{
	// Overflow occured --> Capture must be finished
	//					--> If not -> Error occured
	//							   -> No object detected
	overflowOccured |= 0x01;
	switch(CaptureStatus)
	{
		// If capture stoped, either a good measure or
		// an error (no object found) has occured
		// --> Notify a new measure can be archived
		case CAPTURE_STOPED:
			// new measure availlable, compute
			// duration & put in buffer 
			countDuration = SecondCapture - FirstCapture;
						
			// Handle errors (no object found, small spike)
			if (countDuration < 360)
			{
				countDuration = 0;
			}
						
			averageAddTable(countDuration);
			
		break;
		
		case CAPTURE_STARTED:
		case CAPTURE_WAIT_START:
		case CAPTURE_ERROR:
		default:
		// Error occured -> Flag Error;
		errorOccuredFlag = 0x01;
		break;
	}
	
	// reset flags & registers for new measure
	actualEdge = 0x01;
	TCCR1B |= (1 << ICES1);
	CaptureStatus = CAPTURE_WAIT_START;
}

// CAPTURE
ISR(TIMER1_CAPT_vect)
{
	// State MACHINE for Capture status
	switch (CaptureStatus)
	{
		// If waiting for a measure
		case CAPTURE_WAIT_START:
		// Rising edge must be chosen for this status
		// otherwise, there must be an error
		if (actualEdge == 0x01)
		{
			// Save Timer value of Capture
			FirstCapture = ICR1;
			// Set edge detection for "FALLING EDGE"
			actualEdge = 0x00;
			TCCR1B &= ~(1 << ICES1);
			// Change measure status -> Measure started
			CaptureStatus = CAPTURE_STARTED;
		}
		else
		{
			// ERROR :	Rising edge must be set to start a
			//			measure !
			CaptureStatus = CAPTURE_ERROR;
		}
		break;
		
		case CAPTURE_STARTED:
		// If capture initiated, edge detection should be "FALLING EDGE"
		if (actualEdge == 0x00)
		{
			// Save capture in second register
			SecondCapture = ICR1;
			// Reset edge detection for next measure
			actualEdge = 0x01;
			TCCR1B |= (1 << ICES1);
			// Update of status --> Capture done
			CaptureStatus = CAPTURE_STOPED;
		}
		else
		{
			// ERROR:	Falling edge must be set to end measure
			CaptureStatus = CAPTURE_STOPED;
		}
		break;
		
		case CAPTURE_STOPED:
		case CAPTURE_ERROR:
		CaptureStatus = CAPTURE_ERROR;
		break;
		
		default:
		CaptureStatus = CAPTURE_ERROR;
		break;
	}
}

/* Function : UltraSon_GPIO_INIT(void)
*
*	Input ARG	:	none
*	Returns		:	none	
*
*	Description: 
*		- This function initiates GPIO for this board application
*
*/
void UltraSon_GPIO_INIT(void)
{
	// PORT CONFIGURATION
	DDRB = 0x06;		// PORTB0	- IN  --> ECHO (ICP1)
						// PORTB1	- OUT --> LED USER
						// PORTB2	- OUT --> TRIG
		
	DDRC = 0x1F;		// PORTC0-3	- OUT --> RANGE0..3
						// PORTC4	- OUT --> USER_PUD
						// PORTC5	- IN  --> SEL_MODE
		
	DDRD = 0x7A;		// PORTD0	- IN  <-- UART_RX
						// PORTD1	- OUT --> UART_TX
						// PORTD2	- IN  <-- MCU_READ_BUSY
						// PORTD3	- OUT --> MCU_SET_BUSY
						// PORTD4	- OUT --> RANGE4
						// PORTD5	- OUT --> RANGE4
						// PORTD6	- OUT --> MEAS_OK
						// PORTD7	- IN  <-- GET_MEASURE
}


/*	Function : InitTimer
*
*		Input Arg.	: none
*		Returns		: none
*
*	Description : 
*	
*		- Initialize timer 1	
*			--> Prescaler 8 (1MHz counting frequency --> 1us resolution)
*			--> MODE PWM_OCR
*			--> Input Capture : rising edge
*			--> Pin mode : OC1B set at compare, clear at restart
*				--> duration : 15 ticks (~ 15 us high, ~ 65.52ms low)
*			--> Interrupt : Input Capture + Overflow of Timer 1
*/
void Init_Timer(void)
{
	// TCCR1B -> |X|X|-|>> WGM13 <<|>> WGM12 <<|X|X|X|
	// TCCR1A -> |X|X|X|X|-|-|>> WGM11 <<|>> WGM10 <<|
	
	TCCR1A &= ~(WGM_11_10_MASK);	// Clear old mode (TCCR1A)
	TCCR1A |= (T1_MODE_PWM_OCR & WGM_11_10_MASK);		// write new mode in TCCR1A
	
	TCCR1B &= ~(WGM_13_12_MASK); // Clear old mode (TCCR1B)
	TCCR1B |= ((T1_MODE_PWM_OCR << 1) & WGM_13_12_MASK);	// write new mode in TCCR1B
	
	TCCR1B |= (1 << ICES1);		// Input capture on rising edge
	
	// Pin mode 
	// TCCR1A -> |X|X|X|X|-|-|X|X|
	TCCR1A &= ~(COM_A_B_MASK);	// clear old pin mode
	TCCR1A |= ((0x00 << 6) | (0x03 << 4));	// Pin mode : Set on compare, clear at reset
	
	// Frequency & Trig
	OCR1A = 0xFFFF;		// TOP = max value --> 15.5 measures/sec
	OCR1B = 0xFFF0;		// COM = MAX - 15 --> 15 microseconds for trig
	
	// Activate Interrupt for Input capture
	TIMSK1 |= (1 << ICIE1);
	// Activate Interrupt for Overflow
	TIMSK1 |= (1 << TOIE1);
}

/*	Function : Timer_Start
*
*		Input Arg.	: none
*		Returns		: none
*
*	Description :
*		--> Start counting (sets clock source + Prescaler = 8)
*/
void Timer_Start(void)
{
	TCCR1B |= 0x02;			// Prescaler 8
}

/*	Function : Timer_Stop
*
*		Input Arg.	: none
*		Returns		: none
*
*	Description :
*
*		--> Stop counting for Timer 1 by clearing clock source
*/
void Timer_Stop(void)
{
	TCCR1B &= ~(CS_T0T1_MASK);		// Clear clock source -> stops Timer
}

/*	Function : averageAddTable
*
*		Input Arg.	: NewValue (uint16_t)
*		Returns		: none
*
*	Description :
*		
*		- Add 16bit value received in buffer (rotating, contains only 
*		  "N" last measure taken)
*		- This function also updates "Sumbuffer (uint32_t)". 
*		  this permits to get average much more faster (sum is ready)
*/
void averageAddTable(uint16_t NewValue)
{
	// Add to summ only if flag not rised
	if(flagBufferFull != 0x00)
	{
		// Remove old value
		SumBuffer -= measureBuffer[pMeasBuffer];
		
		// Add to sum new value
		SumBuffer += NewValue;
	}
	else
	{
		// Buffer is filling --> only add new values
		SumBuffer += NewValue;
	}
	
	// Save value in buffer
	measureBuffer[pMeasBuffer] = NewValue;
	
	// incrementation of index
	pMeasBuffer++;
	
	// Flag if buffer has been filled once
	if (pMeasBuffer >= MEAS_BUFFER_LEN)
	{
		flagBufferFull |= 0x01;
	}
	
	// reset position if max length reached
	pMeasBuffer %= MEAS_BUFFER_LEN;

}

/*	Function : getAverageDistance
*
*		Input Arg.	: nSamples (uint8_t)
*		Returns		: averaged value (uint16_t)
*
*	Description :
*		- Add 16bit value received in buffer (rotating, contains only
*		  "N" last measure taken)
*		- This function also updates "Sumbuffer (uint32_t)".
*		  this permits to get average much more faster (sum is ready)
*
*		- This function returns "0" if buffer isn't full 
*/
uint16_t getAverageDistance(uint8_t nSamples)
{
	// Variable for sum
	uint32_t sum = 0;
	uint32_t average = 0;
	int8_t newIndex=0;
	
	if (flagBufferFull != 0x00)
	{
		// if NSample >= length of buffer returns directly
		// the average with the SUM register
		if(nSamples >= MEAS_BUFFER_LEN)
		{
			// no need to compute entire buffer (Sum already exist)
			average = SumBuffer;
			average /= MEAS_BUFFER_LEN;
		}
		// User chose a specific number of sample to be averaged
		else
		{
			// must set new pointer on buffer
			newIndex = pMeasBuffer;
			newIndex -= nSamples;
			
			// if pMeasBuffer is at start, newIndex will be < 0
			// newIndex must be re-calculated considerating
			// buffer rolling behaviour (0 -> end -> 0)
			if(newIndex < 0)
			{
				// Compute for the nSample Last measures
				newIndex = (MEAS_BUFFER_LEN - nSamples) + pMeasBuffer;
			}
			
			// Loop to compute sum of number of sample desired
			for (uint8_t i = 0; i < nSamples; i++)
			{
				sum += measureBuffer[newIndex];
				newIndex++;
				newIndex %= MEAS_BUFFER_LEN;
			}
			
			// Compute average
			average = sum;
			
			// avoid dividing by 0 !
			if (nSamples == 0)
			{
				average = 0;
			}
			else
			{
				average /= nSamples;
			}
		}
	}
	else
	{
		average = 0;
	}
	
	// returne averaged value
	return average;
}