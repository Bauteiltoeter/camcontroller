#include "menu_tally.h"
#include "lcd.h"
#include "tally.h"
#include "globals.h"
#include <stdlib.h>
#include <stdio.h>

void menu_tally_update(void);

void menu_tally_init(void)
{
    menu_tally_update();

    lcd_gotoxy(16,0);
    lcd_puts_P("CAM");
    char tmp[10];
	itoa(active_cam+1,tmp,10);
    lcd_puts(tmp);
}

void menu_tally_run(void)
{
    static uint8_t last_active_cam;

    if(last_active_cam != active_cam)
    {
        last_active_cam = active_cam;
        lcd_gotoxy(16,0);
        lcd_puts_P("CAM");
        char tmp[10];
        itoa(active_cam+1,tmp,10);
        lcd_puts(tmp);
        menu_tally_update();
    }
}


void menu_tally_update(void)
{
    lcd_gotoxy(0,2);
    if(cams[active_cam].tally_mode == tally_auto)
    {
      
        lcd_puts_P("vvvv           "); 
    }
    else if(cams[active_cam].tally_mode == tally_yellow)
    {
        lcd_puts_P("     vvvvv     ");
    }
    else
    {
        
        lcd_puts_P("           vvvv");
    }

    
}

void menu_tally_auto(void)
{
    tally_setAuto();
    menu_tally_update();
}

void menu_tally_yellow(void)
{
    tally_setYellow();
    menu_tally_update();
}

void menu_tally_blue(void)
{
    tally_setBlue();
    menu_tally_update();
}
