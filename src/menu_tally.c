#include "menu_tally.h"
#include "lcd.h"
#include "tally.h"
void menu_tally_init(void)
{
    menu_tally_update();

}


void menu_tally_update(void)
{
    lcd_gotoxy(0,2);
    if(tally_getMode() == tally_auto)
    {
      
        lcd_puts_P("vvvv           "); 
    }
    else if(tally_getMode() == tally_yellow)
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
