/*
 * SD_TEST_LIB.c
 *
 * Created: 22.01.2024 13:01:46
 * Author : adria
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "sd.h"
#include "fat.h"
#include "utils.h"
#include "AVR_Uart.h"

DIR dir;
FILE_SD file;
uint8_t return_code = 0;

int main(void)
{
	uint8_t test = 0;
	
	DDRC = 0x01;
	DDRD = 0x02;
	
	sei();
	UART_Init();
	
	
	while (1)
	{
		if (test == 1)
		{
			test = 0;
			return_code = sd_init();
			
			if (return_code == SD_OK)
			{
				Uart_PutString("SD_OK;");
			}
			else if (return_code == SD_IDLE_STATE_TIMEOUT)
			{
				Uart_PutString("SD_IDLE_STATE_TIMEOUT;");
			}
			else if (return_code == SD_GENERAL_ERROR)
			{
				Uart_PutString("SD_GENERAL_ERROR;");
				Uart_PutString(msgUart01);
			}
			else if (return_code == SD_CHECK_PATTERN_MISMATCH)
			{
				Uart_PutString("SD_CHECK_PATTERN_MISMATCH;");
			}
			else if (return_code == SD_NONCOMPATIBLE_VOLTAGE_RANGE)
			{
				Uart_PutString("SD_NONCOMPATIBLE_VOLTAGE_RANGE;");
			}
			else if (return_code == SD_POWER_UP_BIT_NOT_SET)
			{
				Uart_PutString("SD_POWER_UP_BIT_NOT_SET;");
			}
			else if (return_code == SD_NOT_SD_CARD)
			{
				Uart_PutString("SD_NOT_SD_CARD;");
			}
			else
			{
				Uart_PutString("rien SD;");
			}
			
			return_code = FAT_mountVolume();
			Uart_PutString("mount no;");
			
			// If no error
			if(return_code == MR_OK)
			{
				Uart_PutString("MR_OK;");
				Uart_PutString("mount ok;");
				// Make a directory
				FAT_makeDir("/Logging");
				
				// Open folder
				return_code = FAT_openDir(&dir, "/Logging");
				if(return_code == FR_OK){
					// ... optionally print folder name using an LCD library: print(FAT_getFilename());
					Uart_PutString("open dir ok;");
					
					// Open a file for reading or writing
					return_code = FAT_fopen(&dir, &file, "log.txt");
					
					// Make the file if it doesn't exist
					// The functions that make folders and files takes much space
					// so it is recommended to have them made on a PC instead
					if(return_code == FR_NOT_FOUND){
						Uart_PutString("file n'existe pas;");
						FAT_makeFile("/Logging/log.txt");
						return_code = FAT_fopen(&dir, &file, "log.txt");
					}
					
					// Move the writing pointer to the end of the file
					FAT_fseekEnd(&file);
					
					// Write a string
					FAT_fwriteString(&file, "Logging Date: 2022\n");
					
					// Write sensor output
					FAT_fwriteFloat(&file, 120.033, 3);
					FAT_fwriteString(&file, ",");
					FAT_fwriteFloat(&file, -0.221, 3);
					FAT_fwriteString(&file, ",");
					FAT_fwriteFloat(&file, -30.004, 3);
					FAT_fwriteString(&file, ",");
					FAT_fwriteFloat(&file, 0.023, 3);
					
					// Synchronize the writing buffer with the card
					FAT_fsync(&file);
					Uart_PutString("OK");
				}
			}
			else if (return_code == MR_DEVICE_INIT_FAIL)
			{
				Uart_PutString("MR_DEVICE_INIT_FAIL;");
			}
			else if (return_code == MR_ERR)
			{
				Uart_PutString("MR_ERR;");
			}
			else if (return_code == MR_NO_PARTITION)
			{
				Uart_PutString("MR_NO_PARTITION;");
			}
			else if (return_code == MR_FAT_ERR)
			{
				Uart_PutString("MR_FAT_ERR;");
				Uart_PutString(msgUart0);
				Uart_PutString(msgUart1);
			}
			else if (return_code == MR_UNSUPPORTED_FS)
			{
				Uart_PutString("MR_UNSUPPORTED_FS;");
			}
			else if (return_code == MR_UNSUPPORTED_BS)
			{
				Uart_PutString("MR_UNSUPPORTED_BS;");
			}
			else
			{
				Uart_PutString("rien mount;");
			}
			
			Uart_PutString("---------------------;");
			
			while (PINC & (1 << PINC0))
			{
				;
			}
		}
		
		if (PINC & (1 << PINC0))
		{
			test = 1;
		}
	}
}

