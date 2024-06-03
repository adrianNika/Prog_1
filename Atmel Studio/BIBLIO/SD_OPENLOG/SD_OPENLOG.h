/*
 * SD_ONPENLOG.h
 *
 * Created: 12.02.2024 10:07:28
 *  Author: adria
 */ 


#ifndef SD_OPENLOG_H_
#define SD_OPENLOG_H_

#include "AVR_Uart.h"
#include "global.h"

uint8_t Openlog_Init();
uint8_t Openlog_AppendStringToFile(char * str, char * filename);
uint8_t Openlog_SetCmdMode();
uint8_t Openlog_CheckIfCmdMode();

#endif /* SD_OPENLOG_H_ */