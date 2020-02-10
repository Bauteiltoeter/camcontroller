#include "menu_main.h"
#include "globals.h"
#include "rotary.h"
#include "hardware.h"
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>

static const char PARAM_SHUTTER[] PROGMEM		= "Shutter    ";
static const char PARAM_R[] PROGMEM 			= "Red        ";
static const char PARAM_G[] PROGMEM				= "Green      ";
static const char PARAM_B[] PROGMEM				= "Blue       ";
static const char PARAM_FOCUS[] PROGMEM			= "Focus      ";
static const char PARAM_ROTF[] PROGMEM			= "RotFunction";
static const char PARAM_ROT[] PROGMEM			= "Rotation   ";

typedef struct {
	const char* name;
} fixture_param_t;

__flash const fixture_param_t fixture_params[] =
{
	{
		.name = PARAM_SHUTTER
	},
	{
		.name = PARAM_R
	},
	{
		.name = PARAM_G
	},
	{
		.name = PARAM_B
	},
	{
		.name = PARAM_FOCUS
	},
	{
		.name = PARAM_ROTF
	},
	{
		.name = PARAM_ROT
	}
};

static uint8_t active_param;

void main_init(void)
{
	main_show();
}

void main_show(void)
{


}	


void main_run(void)
{
    fselkey_t fsel = get_fselkeys();

    if(fsel != FSEL_NO_KEY)
	{
		switch(fsel)
		{
			case FSEL1: set_fsel_leds(1); active_param = 0;break;
			case FSEL2: set_fsel_leds(2); active_param = 1;break;
			case FSEL3: set_fsel_leds(3); active_param = 2;break;
			case FSEL4: set_fsel_leds(4); active_param = 3;break;
			case FSEL5: set_fsel_leds(5); active_param = 4;break;
			case FSEL6: set_fsel_leds(6); active_param = 5;break;
			case FSEL7: set_fsel_leds(7); active_param = 6;break;
			case FSEL8: set_fsel_leds(8); active_param = 7;break;
		}

		lcd_gotoxy(0,0);
		lcd_puts_p(fixture_params[active_param].name);
	}
}