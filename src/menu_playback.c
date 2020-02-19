#include "menu_playback.h"
#include <stdint.h>
#include "scenestorage.h"
#include "lcd.h"
uint8_t seconds;
uint8_t activeScene;
uint8_t nextScene;
uint16_t activeTotalTime;
uint8_t activeTiming;
uint8_t activeCrossfade;
uint8_t disable;
uint8_t numberOfScenes;
uint8_t nextSceneIndex;

void crossfade_calculate(void);
void crossfade_init(void);

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
        nextScene=1; //TODO search for next used
        activeTiming = scenestorage_getTiming(activeScene);
        activeCrossfade = scenestorage_getCrossfade(activeScene);
        activeTotalTime = activeTiming + activeCrossfade;
        scenestorage_load(activeScene);
        crossfade_init();

        lcd_gotoxy(15,1);
        char tmp[20];
        sprintf(tmp,"%d/%d",activeScene+1,numberOfScenes);
        lcd_puts(tmp);
    }
}

void playback_update()
{
    static uint8_t ms=0;
    char tmp[20];
    ms++;

    if(disable)
        return;

    //called every 10ms


        seconds++; 


    if(activeTiming > 0)
    {   if(ms>100)
        {
            activeTiming--;
            lcd_gotoxy(0,2);
            sprintf(tmp,"Seconds remain: %03d",activeTiming);
            lcd_puts(tmp);
        }
    }
    else
    {
        if(activeCrossfade > 0)
        {
            crossfade_calculate();
            if(ms>100)
            {
                activeCrossfade--;
                lcd_gotoxy(0,2);
                sprintf(tmp,"Seconds crossf: %03d",activeCrossfade);
                lcd_puts(tmp);
            }
        }
        else
        {
            if(ms>100)
            {
                //Load next
                activeScene= nextScene;
                do
                {
                    nextScene++;
                    if(nextScene>=SCENE_COUNT)
                        nextScene=0;
                }
                while(!scenestorage_hasData(nextScene));
                scenestorage_load(activeScene);
                activeTotalTime = scenestorage_getTiming(activeScene) + (uint16_t)scenestorage_getCrossfade(activeScene);
                activeCrossfade = scenestorage_getCrossfade(activeScene);
                activeTiming = scenestorage_getTiming(activeScene);
                lcd_gotoxy(14,1);
                sprintf(tmp,"%d/%d:%d",activeScene+1,numberOfScenes,nextScene);
                lcd_puts(tmp);
                lcd_gotoxy(0,2);
                sprintf(tmp,"Seconds remain: %03d",activeTiming);
                lcd_puts(tmp);
                crossfade_init();
            }
        }
            
    }

    if(ms>100)
        ms=0;
}

float cross_test;
float cross_test_value;

typedef struct {
    float step;
    float value;
    uint8_t* dmx;
    uint8_t* next;
} crossfade_u8_t;


void crossfade_calculate_uint8(crossfade_u8_t* crossfade);

scene_t nextSceneData;

crossfade_u8_t crossfade_u8[] =  {
    { .dmx = &imageScan_data[0].focus, .next=&nextSceneData.imagescan[0].focus},
    { .dmx = &imageScan_data[0].rotf, .next=&nextSceneData.imagescan[0].rotf},
    { .dmx = &imageScan_data[0].rot, .next=&nextSceneData.imagescan[0].rot},

    { .dmx = &imageScan_data[1].focus, .next=&nextSceneData.imagescan[1].focus},
    { .dmx = &imageScan_data[1].rotf, .next=&nextSceneData.imagescan[1].rotf},
    { .dmx = &imageScan_data[1].rot, .next=&nextSceneData.imagescan[1].rot},

    { .dmx = &cameotri_data[0].red, .next=&nextSceneData.cameotri[0].red},
    { .dmx = &cameotri_data[0].green, .next=&nextSceneData.cameotri[0].green},
    { .dmx = &cameotri_data[0].blue, .next=&nextSceneData.cameotri[0].blue},
    { .dmx = &cameotri_data[0].dimmer, .next=&nextSceneData.cameotri[0].dimmer},
    
    { .dmx = &cameotri_data[1].red, .next=&nextSceneData.cameotri[1].red},
    { .dmx = &cameotri_data[1].green, .next=&nextSceneData.cameotri[1].green},
    { .dmx = &cameotri_data[1].blue, .next=&nextSceneData.cameotri[1].blue},
    { .dmx = &cameotri_data[1].dimmer, .next=&nextSceneData.cameotri[1].dimmer},

    { .dmx = &cameotri_data[2].red, .next=&nextSceneData.cameotri[2].red},
    { .dmx = &cameotri_data[2].green, .next=&nextSceneData.cameotri[2].green},
    { .dmx = &cameotri_data[2].blue, .next=&nextSceneData.cameotri[2].blue},
    { .dmx = &cameotri_data[2].dimmer, .next=&nextSceneData.cameotri[2].dimmer},

    { .dmx = &cameotri_data[3].red, .next=&nextSceneData.cameotri[3].red},
    { .dmx = &cameotri_data[3].green, .next=&nextSceneData.cameotri[3].green},
    { .dmx = &cameotri_data[3].blue, .next=&nextSceneData.cameotri[3].blue},
    { .dmx = &cameotri_data[3].dimmer, .next=&nextSceneData.cameotri[3].dimmer},

    { .dmx = &cameotri_data[4].red, .next=&nextSceneData.cameotri[4].red},
    { .dmx = &cameotri_data[4].green, .next=&nextSceneData.cameotri[4].green},
    { .dmx = &cameotri_data[4].blue, .next=&nextSceneData.cameotri[4].blue},
    { .dmx = &cameotri_data[4].dimmer, .next=&nextSceneData.cameotri[4].dimmer},

    { .dmx = &cameotri_data[5].red, .next=&nextSceneData.cameotri[5].red},
    { .dmx = &cameotri_data[5].green, .next=&nextSceneData.cameotri[5].green},
    { .dmx = &cameotri_data[5].blue, .next=&nextSceneData.cameotri[5].blue},
    { .dmx = &cameotri_data[5].dimmer, .next=&nextSceneData.cameotri[5].dimmer},

    { .dmx = &cameoflood_data[0].red, .next=&nextSceneData.cameoflood[0].red},
    { .dmx = &cameoflood_data[0].green, .next=&nextSceneData.cameoflood[0].green},
    { .dmx = &cameoflood_data[0].blue, .next=&nextSceneData.cameoflood[0].blue},
    { .dmx = &cameoflood_data[0].white, .next=&nextSceneData.cameoflood[0].white},
    { .dmx = &cameoflood_data[0].amber, .next=&nextSceneData.cameoflood[0].amber},
    { .dmx = &cameoflood_data[0].uv, .next=&nextSceneData.cameoflood[0].uv},
    { .dmx = &cameoflood_data[0].dimmer, .next=&nextSceneData.cameoflood[0].dimmer},

    { .dmx = &cameoflood_data[1].red, .next=&nextSceneData.cameoflood[1].red},
    { .dmx = &cameoflood_data[1].green, .next=&nextSceneData.cameoflood[1].green},
    { .dmx = &cameoflood_data[1].blue, .next=&nextSceneData.cameoflood[1].blue},
    { .dmx = &cameoflood_data[1].white, .next=&nextSceneData.cameoflood[1].white},
    { .dmx = &cameoflood_data[1].amber, .next=&nextSceneData.cameoflood[1].amber},
    { .dmx = &cameoflood_data[1].uv, .next=&nextSceneData.cameoflood[1].uv},
    { .dmx = &cameoflood_data[1].dimmer, .next=&nextSceneData.cameoflood[1].dimmer},

    { .dmx = &cameoflood_data[2].red, .next=&nextSceneData.cameoflood[2].red},
    { .dmx = &cameoflood_data[2].green, .next=&nextSceneData.cameoflood[2].green},
    { .dmx = &cameoflood_data[2].blue, .next=&nextSceneData.cameoflood[2].blue},
    { .dmx = &cameoflood_data[2].white, .next=&nextSceneData.cameoflood[2].white},
    { .dmx = &cameoflood_data[2].amber, .next=&nextSceneData.cameoflood[2].amber},
    { .dmx = &cameoflood_data[2].uv, .next=&nextSceneData.cameoflood[2].uv},
    { .dmx = &cameoflood_data[2].dimmer, .next=&nextSceneData.cameoflood[2].dimmer},

    { .dmx = &citycolor_data[0].cyan, .next=&nextSceneData.citycolor[0].cyan},
    { .dmx = &citycolor_data[0].magenta, .next=&nextSceneData.citycolor[0].magenta},
    { .dmx = &citycolor_data[0].yellow, .next=&nextSceneData.citycolor[0].yellow},

    { .dmx = &citycolor_data[1].cyan, .next=&nextSceneData.citycolor[1].cyan},
    { .dmx = &citycolor_data[1].magenta, .next=&nextSceneData.citycolor[1].magenta},
    { .dmx = &citycolor_data[1].yellow, .next=&nextSceneData.citycolor[1].yellow},

    { .dmx = &citycolor_data[2].cyan, .next=&nextSceneData.citycolor[2].cyan},
    { .dmx = &citycolor_data[2].magenta, .next=&nextSceneData.citycolor[2].magenta},
    { .dmx = &citycolor_data[2].yellow, .next=&nextSceneData.citycolor[2].yellow},
};

#define CROSSFADE_U8_SIZE (sizeof(crossfade_u8)/sizeof(crossfade_u8_t))

void crossfade_calculate(void)
{
    for(uint8_t i=0; i < CROSSFADE_U8_SIZE; i++)
    {
        crossfade_calculate_uint8(&crossfade_u8[i]);
    }
}

void crossfade_init(void)
{
    scenestorage_getScene(nextScene,&nextSceneData);

    for(uint8_t i=0; i < CROSSFADE_U8_SIZE; i++)
    {
        crossfade_u8[i].step = (*crossfade_u8[i].next- (float)(*crossfade_u8[i].dmx)) / (float)(scenestorage_getCrossfade(activeScene)*100.0);
        crossfade_u8[i].value = *crossfade_u8[i].dmx;
    }


//    if(scenestorage_getCrossfade(activeScene)>0)
//        cross_test =  (next->imagescan[0].shutter - (float)imageScan_data[0].shutter) / (float)(scenestorage_getCrossfade(activeScene)*100.0);
//    else
//    {
//        cross_test=0;
//    }
//
//    cross_test_value = imageScan_data[0].shutter;

    char tmp[20];
    //sprintf(tmp,"Ah:%f",crossfade_u8[0].step);
    //lcd_gotoxy(0,3);
    //lcd_puts(tmp);
    
}



void crossfade_calculate_uint8(crossfade_u8_t* crossfade)
{
    crossfade->value += crossfade->step;

    if(crossfade->value < 0 )
        crossfade->value = 0;
    if(crossfade->value > 255)
        crossfade->value=255;
    
    *crossfade->dmx=(uint8_t)crossfade->value;

        char tmp[20];
    //sprintf(tmp,"Ahhh:%f",crossfade->value);
    //lcd_gotoxy(0,3);
    //lcd_puts(tmp);
}