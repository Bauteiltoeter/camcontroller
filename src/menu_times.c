#include <stdint.h>
#include "menu_times.h"
#include "hardware.h"
#include "scenestorage.h"
#include "lcd.h"
#include "rotary.h"
uint16_t currentTiming;
uint16_t currentCrossfade;
uint8_t currentSelection;
uint8_t currentScene;

static void updateLcd(void);
static void prevScene(void);
static void nextScene(void);
static void loadScene(void);

static rotary_config_t rotconfig = {
    .data_u16 = 0,
    .type = rotary_uint16,
    .min = 0,
    .max = 6000,
    .change = updateLcd,
    .multi = 1,
    .fastMulti = 20,
    .wrap = 0,
    .leds_on = 1
};

void times_init(void)
{
    currentSelection=0;
    loadScene();
    currentScene=0;
    updateLcd();

    rotconfig.data_u16 = &currentCrossfade;
    rotary_setconfig(&rotconfig);

}

void times_update(void)
{

	fixturekey_t fixsel = get_fixturekeys();
    static uint8_t lastPress = 0;
	if(fixsel != FIX_NO_KEY)
	{
		if(!lastPress)
		{
			switch(fixsel)
			{
				case FIX1: prevScene(); break;
				case FIX2: nextScene(); break;
				case FIX3: break;
				case FIX4: break;
			}
		}
		lastPress = 1;
	}
	else
	{
		lastPress = 0;
	}


}

static void updateLcd(void)
{
    if(currentSelection==0)
    {
        lcd_gotoxy(0,1);
        lcd_putc('>');
        //lcd_gotoxy(6,3);
        //lcd_putc('<');

        lcd_gotoxy(0,2);
        lcd_putc(' ');
        //lcd_gotoxy(15,3);
        //lcd_putc(' ');
    }
    else
    {
        lcd_gotoxy(0,1);
        lcd_putc(' ');
        //lcd_gotoxy(6,3);
        //lcd_putc(' ');

        lcd_gotoxy(0,2);
        lcd_putc('>');
        //lcd_gotoxy(15,3);
        //lcd_putc('<');
    }

    char tmp[20];
    lcd_gotoxy(16,0);
    sprintf(tmp,"%2d", currentScene+1);
    lcd_puts(tmp);

    if(scenestorage_hasData(currentScene))
    {
        lcd_gotoxy(1,1);
        sprintf(tmp,"Cross: %6u.%01us", currentCrossfade/10, currentCrossfade%10);
        lcd_puts(tmp);

        lcd_gotoxy(1,2);
        sprintf(tmp,"Time %6u.%01us", currentTiming/10, currentTiming%10);
        lcd_puts(tmp);
    }
    else
    {
        lcd_gotoxy(0,1);
        lcd_puts("No Data             ");

        lcd_gotoxy(0,2);
        lcd_puts("                    ");
    }
    
    
}


void times_cross(void)
{
    currentSelection=0;

    rotconfig.data_u16 = &currentCrossfade;
    rotary_setconfig(&rotconfig);
    updateLcd();
}

void times_time(void)
{
    currentSelection=1;

    rotconfig.data_u16 = &currentTiming;
    rotary_setconfig(&rotconfig);
    updateLcd();
}


static void prevScene(void)
{
    times_save();
    currentScene--;

    if(currentScene>SCENE_COUNT)
    {
        currentScene = SCENE_COUNT-1;
    }
    loadScene();
    updateLcd();
}

static void nextScene(void)
{
    times_save();
    currentScene++;

    if(currentScene>=SCENE_COUNT)
    {
        currentScene = 0;
    }
    loadScene();
    updateLcd();
}

static void loadScene(void)
{
    currentTiming = scenestorage_getTiming(currentScene);
    currentCrossfade = scenestorage_getCrossfade(currentScene);
}


void times_save(void)
{
    scenestorage_setCrossfade(currentScene, currentCrossfade);
    scenestorage_setTiming(currentScene,currentTiming);
}


void times_play(void)
{

}