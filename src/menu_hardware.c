#include "menu_hardware.h"
#include "lcd.h"
#include "adc.h"
#include "hardware.h"
#include <util/delay.h>
#include <stdio.h>

void show_hardware_info(void)
{
	lcd_gotoxy(0,0);

	char line[21];
	sprintf(line, "Matrix: %02X %02X %02X %02X",get_matrix_line(0),get_matrix_line(1),get_matrix_line(2),get_matrix_line(3));		
	lcd_puts(line);
	sprintf(line, "ADC X: %d", ADC_Read(0));
	lcd_gotoxy(6,1);
	lcd_puts("             ");
	lcd_gotoxy(0,1);
	lcd_puts(line);
	sprintf(line,"Y: %d", ADC_Read(1));
	lcd_gotoxy(12,1);
	lcd_puts(line);

	static uint8_t i=0;
	static uint8_t up=1;
	
	set_rotarys_leds(i);
	
	if(up)
	{
		i++;
		if(i>31)
		{
			up=0;
		}
	}
	else
	{
		i--;
		if(i==0)
		{
			up=1;
		}
	}
	

}
