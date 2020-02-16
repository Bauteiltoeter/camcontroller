#include "menu_playback.h"
#include <stdint.h>
#include "scenestorage.h"
#include "lcd.h"
uint8_t seconds;
uint8_t activeScene;
uint8_t activeTiming;
uint8_t disable;
uint8_t numberOfScenes;

void playback_init()
{
    disable = 0;
    numberOfScenes=0;
    for(uint8_t i=0; i < SCENE_COUNT; i++)
    {
        if(scenestorage_hasData(i))
        {
            numberOfScenes++;
        }
    }

    if(numberOfScenes==0)
    {
        disable=1;
        lcd_gotoxy(0,1);
        lcd_puts("No scenes set!");
    }
    else
    {
        activeScene=0;
        activeTiming = scenestorage_getTiming(activeScene);
        scenestorage_load(activeScene);
    }
}

void playback_update()
{
    static uint8_t ms=0;
    ms++;

    if(disable)
        return;

    if(ms>110)
    {
        ms=0;
        seconds++; 
        activeTiming--;

        if(activeTiming==0)
        {
            do
            {
                activeScene++;
                if(activeScene>=SCENE_COUNT)
                    activeScene=0;
                scenestorage_load(activeScene);
                activeTiming = scenestorage_getTiming(activeScene);
            }
            while(!scenestorage_hasData(activeScene));
        }

        lcd_gotoxy(15,1);
        char tmp[20];
        sprintf(tmp,"%d/%d",activeScene+1,numberOfScenes);
        lcd_puts(tmp);

        lcd_gotoxy(0,2);
        sprintf(tmp,"Seconds remain: %03d",activeTiming);
        lcd_puts(tmp);


        //lcd_gotoxy(0,1);
        //sprintf(tmp,"%03d",seconds);
        //lcd_puts(tmp);


    }
}
