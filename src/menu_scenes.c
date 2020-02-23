#include "menu_scenes.h"
#include "scenestorage.h"
#include <stdint.h>
#include "lcd.h"
static uint8_t currentSceneIndex=0;

static void updateLcd(void);

rotary_config_t rotary_scenes_menu = {
	.data_u8 = &currentSceneIndex,
	.type = rotary_uint8,
	.min = 0,
	.max = SCENE_COUNT-1,
	.change = updateLcd,
	.multi = 1,
    .fastMulti = 2,
	.wrap = 1,
	.leds_on = 1
};

void scenes_init(void)
{
    updateLcd();
}


static void updateLcd(void)
{
    lcd_gotoxy(0,1);
    char tmp[20];
    sprintf(tmp,"Scene %2d",currentSceneIndex+1);
    lcd_puts(tmp);
    lcd_gotoxy(0,2);
    if(!scenestorage_hasData(currentSceneIndex))
        lcd_puts("No data");
    else
        lcd_puts("        ");
}


void scenes_load(void)
{
    if(scenestorage_hasData(currentSceneIndex))
    {
        scenestorage_load(currentSceneIndex);
        lcd_gotoxy(0,2);
        lcd_puts("loaded");
    }
}

void scenes_save(void)
{
    lcd_gotoxy(0,2);
    lcd_puts("Saving  ");
    scenestorage_saveTo(currentSceneIndex);
    lcd_gotoxy(0,2);
    lcd_puts("Complete");
}


void scenes_delete(void)
{
    scenestorage_delete(currentSceneIndex);
    updateLcd();
}