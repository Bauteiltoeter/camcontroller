#include "globals.h"
#include "lcd.h"


uint8_t ctrl_selected_cam=0;


void ctrl_cam_up(void)
{
	ctrl_selected_cam++;

	if(ctrl_selected_cam>=CAM_COUNT)
	{
		ctrl_selected_cam=0;
	}

	ctrl_cam_show();

}

void ctrl_cam_down(void)
{
	ctrl_selected_cam--;

	if(ctrl_selected_cam>=CAM_COUNT)
	{
		ctrl_selected_cam=CAM_COUNT-1;
	}

	ctrl_cam_show();
}

void ctrl_cam_show(void)
{
	lcd_gotoxy(0,1);
	lcd_puts("Controll CAM ");
	
	lcd_gotoxy(13,1);
	char str[10];
	itoa(ctrl_selected_cam+1, str, 10);
	lcd_puts(str);
}

void cam_power_on(void)
{
	cams[ctrl_selected_cam].power_state=1;
	cam_power_show();
}

void cam_power_off(void)
{
	cams[ctrl_selected_cam].power_state=0;
	cam_power_show();
}

void cam_button_press(void)
{
	cams[ctrl_selected_cam].button_state=1;
	cam_power_show();
}

void cam_button_release(void)
{
	cams[ctrl_selected_cam].button_state=0;
	cam_power_show();
}

void cam_power_show(void)
{
	if(cams[ctrl_selected_cam].power_state)
	{
		lcd_gotoxy(0,3);
		lcd_putc('>');
		lcd_gotoxy(3,3);
		lcd_putc('<');
		lcd_putc(' ');
		lcd_gotoxy(8,3);
		lcd_putc(' ');
	}
	else
	{
		lcd_gotoxy(0,3);
		lcd_putc(' ');
		lcd_gotoxy(3,3);
		lcd_putc(' ');
		lcd_putc('>');
		lcd_gotoxy(8,3);
		lcd_putc('<');
	}

	if(cams[ctrl_selected_cam].button_state)
	{
		cams[ctrl_selected_cam].button_state=1;
		lcd_gotoxy(10,3);
		lcd_putc('>');
		lcd_gotoxy(14,3);
		lcd_putc('<');

	}
	else
	{
		lcd_gotoxy(10,3);
		lcd_putc(' ');
		lcd_gotoxy(14,3);
		lcd_putc(' ');

	}

	send_switch();
}

void send_switch(void)
{
	uint8_t dmx;

	if(!cams[ctrl_selected_cam].button_state && !cams[ctrl_selected_cam].power_state)
		dmx=0;
	else if(!cams[ctrl_selected_cam].button_state && cams[ctrl_selected_cam].power_state)
		dmx=64;
	else if(cams[ctrl_selected_cam].button_state && !cams[ctrl_selected_cam].power_state)
		dmx=128;
	else if(cams[ctrl_selected_cam].button_state && cams[ctrl_selected_cam].power_state)
		dmx=255;


	write_dmx(cams[ctrl_selected_cam].base_addr + cams[ctrl_selected_cam].switch_address, dmx);
}
