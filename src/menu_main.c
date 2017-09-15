#include "menu_main.h"
#include "globals.h"
#include "rotary.h"
#include <stdio.h>

void main_init(void)
{
	static rotary_config_t rotary_speed_config = 
		{
			.data_u8 = &cams[0].speed,
			.type = rotary_uint8,
			.min = 0,
			.max = 255,
			.change = main_show,
			.multi = 5,
			.wrap = 0
		};

	rotary_speed_config.data_u8 = &cams[active_cam].speed;
	rotary_setconfig(&rotary_speed_config);


	main_show();
}

void main_show(void)
{
	lcd_gotoxy(0,0);
	lcd_puts("Active: CAM ");
	char tmp[21];
	itoa(active_cam+1,tmp,10);
	lcd_gotoxy(12,0);
	lcd_puts(tmp);

	lcd_gotoxy(0,1);
	sprintf(tmp,"Pan: %3d Tilt: %3d", cams[active_cam].pan, cams[active_cam].tilt);
	lcd_puts(tmp);

	lcd_gotoxy(0,2);
	sprintf(tmp,"Speed: %3d", cams[active_cam].speed);
	lcd_puts(tmp);
}	
