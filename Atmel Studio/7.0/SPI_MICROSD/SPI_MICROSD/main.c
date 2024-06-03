
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "fat.h"
#include "sd.h"
#include "utils.h"


DIR dir;
FILE_SD file;
uint8_t return_code = 0;
uint16_t dirItems = 0;

int main(void)
{
	SPI_Init();
	sd_init();
	
	for (int i = 0; i < 6; i++)
	{
		PORTD ^= 0x04;
		_delay_ms(200);
	}
	
	
	while(1)
	{
		FAT_mountVolume();
		
		FAT_makeDir("/test");
		
		FAT_openDir(&dir, "/test");
		
		FAT_fdeleteByName(&dir, "log.txt");
		
		FAT_makeFile("/test/log.txt");
		
		FAT_fopen(&dir, &file, "log.txt");
		
		FAT_fseekEnd(&file);
		
		FAT_fwriteString(&file, "Battery Tester\n");
		FAT_fwriteFloat(&file, 120.033, 3);
		
		FAT_fsync(&file);
		
		while(1);
	}
}	
