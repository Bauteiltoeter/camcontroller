#include "menu_dmxaddr.h"
#include "lcd.h"
#include "globals.h"
#include "rotary.h"
typedef struct {
    uint16_t* dmx_addr;
    char* name;
    uint8_t channelcount;
} dmx_setup_t;

__flash const dmx_setup_t dmx_setup[] = {
    {
        .dmx_addr = &imageScan_data[0].dmx_addr,
        .name = "ImageScan 1 (9CH)",
        .channelcount = 9
    },
    {
        .dmx_addr = &imageScan_data[1].dmx_addr,
        .name = "ImageScan 2 (9CH)",
        .channelcount = 9
    },
    {
        .dmx_addr = &cameotri_data[0].dmx_addr,
        .name = "Cameo Tri 1 (6CH)",
        .channelcount = 6
    },
    {
        .dmx_addr = &cameotri_data[1].dmx_addr,
        .name = "Cameo Tri 2 (6CH)",
        .channelcount = 6
    },
    {
        .dmx_addr = &cameotri_data[2].dmx_addr,
        .name = "Cameo Tri 3 (6CH)",
        .channelcount = 6
    },
    {
        .dmx_addr = &cameotri_data[3].dmx_addr,
        .name = "Cameo Tri 4 (6CH)",
        .channelcount = 6
    },
    {
        .dmx_addr = &cameotri_data[4].dmx_addr,
        .name = "Cameo Tri 5 (6CH)",
        .channelcount = 6
    },
    {
        .dmx_addr = &cameotri_data[5].dmx_addr,
        .name = "Cameo Tri 6 (6CH)",
        .channelcount = 6
    },
    {
        .dmx_addr = &cameoflood_data[0].dmx_addr,
        .name = "Cameo Flood 1 (9CH)",
        .channelcount = 9
    },
    {
        .dmx_addr = &cameoflood_data[1].dmx_addr,
        .name = "Cameo Flood 2 (9CH)",
        .channelcount = 9
    },
    {
        .dmx_addr = &cameoflood_data[2].dmx_addr,
        .name = "Cameo Flood 3 (9CH)",
        .channelcount = 9
    },
    {
        .dmx_addr = &citycolor_data[0].dmx_addr,
        .name = "CityColor 1 (7CH)",
        .channelcount = 7
    },
    {
        .dmx_addr = &citycolor_data[1].dmx_addr,
        .name = "CityColor 2 (7CH)",
        .channelcount = 7
    },
    {
        .dmx_addr = &citycolor_data[2].dmx_addr,
        .name = "CityColor 3 (7CH)",
        .channelcount = 7
    },
};

#define DMX_SETUP_SIZE (sizeof(dmx_setup)/sizeof(dmx_setup_t))
static uint8_t currentDevice=0;

void dmxaddr_updateValue();

rotary_config_t dmxaddr_rotary = {
	.data_u16 = 0,
	.type = rotary_uint16,
	.min = 1,
	.max = 512,
	.change = dmxaddr_updateValue,
	.multi = 1,
	.wrap = 1,
	.leds_on = 1
};

void dmxaddr_init(void)
{
    lcd_gotoxy(0,1);
    lcd_puts("                    ");
    lcd_gotoxy(0,1);
    lcd_puts(dmx_setup[currentDevice].name);
    lcd_gotoxy(0,2);
    lcd_puts("Address: ");
    dmxaddr_updateValue();

    dmxaddr_rotary.data_u16=dmx_setup[currentDevice].dmx_addr;
    rotary_setconfig(&dmxaddr_rotary);
}


void dmxaddr_up(void)
{
   (*dmx_setup[currentDevice].dmx_addr)++;
   dmxaddr_updateValue();


    if((*dmx_setup[currentDevice].dmx_addr)>=512)
        (*dmx_setup[currentDevice].dmx_addr) = 0;
}

void dmxaddr_down(void)
{

   (*dmx_setup[currentDevice].dmx_addr)--;

    if((*dmx_setup[currentDevice].dmx_addr)==0)
        (*dmx_setup[currentDevice].dmx_addr) = 512;
   
   dmxaddr_updateValue();
}

void dmxaddr_next(void)
{
    currentDevice++;
    if(currentDevice>=DMX_SETUP_SIZE)
    {
        currentDevice=0;
    }
    dmxaddr_init();

}

void dmxaddr_updateValue()
{
    lcd_gotoxy(9,2);
    char tmp[20];
    sprintf(tmp,"%03d",*(dmx_setup[currentDevice].dmx_addr));
    lcd_puts(tmp);
}


void dmxaddr_auto(void)
{
    if(currentDevice==0)
        *(dmx_setup[currentDevice].dmx_addr)=1;
    else
    {
        *(dmx_setup[currentDevice].dmx_addr)= *(dmx_setup[currentDevice-1].dmx_addr) + dmx_setup[currentDevice-1].channelcount; 
    }

    dmxaddr_updateValue();
    

}