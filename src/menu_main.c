#include "menu_main.h"
#include "globals.h"
#include "rotary.h"
#include "hardware.h"
#include "lcd.h"
#include "adc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>



static const char NAME_IMAGESCAN[] PROGMEM		= "ImageScan";
static const char NAME_CAMEO_TRI[] PROGMEM		= "Cameo TRI Par";
static const char NAME_CAMEO_FLOOD[] PROGMEM	= "Cameo Flood";
static const char NAME_CITYCOLOR[] PROGMEM		= "Citycolor";

static const char PARAM_FOCUS[] PROGMEM			= "Focus";
static const char PARAM_ROTF[] PROGMEM			= "RotFunction";
static const char PARAM_ROT[] PROGMEM			= "Rotation";
static const char PARAM_SHUTTER[] PROGMEM		= "Shutter";
static const char PARAM_R[] PROGMEM 			= "Red";
static const char PARAM_G[] PROGMEM				= "Green";
static const char PARAM_B[] PROGMEM				= "Blue";
static const char PARAM_A[] PROGMEM				= "Amber";
static const char PARAM_UV[] PROGMEM	     	= "UV";
static const char PARAM_STROBE[] PROGMEM		= "Strobe";
static const char PARAM_DIMMER[] PROGMEM		= "Dimmer";
static const char PARAM_CYAN[] PROGMEM			= "Cyan";
static const char PARAM_MAGENT[] PROGMEM		= "Magenta";
static const char PARAM_YELLOW[] PROGMEM		= "Yellow";
static const char PARAM_WHITE[] PROGMEM			= "White";
static const char PARAM_DURATION[] PROGMEM		= "Duration";
static const char PARAM_SPEED[] PROGMEM			= "Speed";
static const char PARAM_BASICCOLOR[] PROGMEM    = "Color";
static const char PARAM_LAMP[] PROGMEM    		= "Lamp";


typedef enum {
	fixture_imageScan,
	fixture_cameo
} fixture_type_t;



typedef struct  {
	uint8_t* baseAddr;
	size_t arrayMemberSize;
	size_t parameterOffset;
	parameter_type parameter_type;
}fixture_parameter_t;

typedef struct {
	const char* name;
	const char* fname[14];
	fixture_parameter_t* parameterConfig[14];
	//rotary_config_t* rotConfig[8];
} fixture_parameterset_t;

typedef enum  {
	imageScan = 0,
	cameo_tri = 1,
	cameo_flood = 2,
	citycolor = 3
} fixture_parametersetindex;

typedef enum {
	selected_none,
	selected_imagescan,
	selected_tri,
	selected_flood,
	selected_citycolor
} selected_status_t;

static selected_status_t selected_status = selected_none;
//uint8_t shutterValue;
//uint8_t focusValue;
static uint8_t active_param;
static uint8_t fixture_selected;
static fixture_parametersetindex parameterSetIndex;
static uint8_t parameterBank=0;

static void updateConfig(void);
static void updateLcd(void);
static uint8_t getCurrentIndex(void);
static void nextFixture(void);
static void previousFixture(void);
static void select_pressed(void);
static void deselectAll_pressed(void);
static void updateSelectionLEDs(void);

//static void updateImageScanShutter(void);
//static void updateImageScanFocus(void);
/*const rotary_config_t rotary_imagescan_shutter = {
	.data_u8 = &shutterValue,
	.type = rotary_uint8,
	.min = 0,
	.max = 255,
	.change = updateImageScanShutter,
	.multi = 1,
	.wrap = 0,
	.leds_on = 1
};

const rotary_config_t rotary_imagescan_focus = {
	.data_u8 = &focusValue,
	.type = rotary_uint8,
	.min = 0,
	.max = 255,
	.change = updateImageScanFocus,
	.multi = 1,
	.wrap = 0,
	.leds_on = 1
};*/

uint8_t* get_param_u8_addr (uint8_t index);

const  fixture_parameter_t param_imagescan_shutter = {
	.baseAddr = (uint8_t*)imageScan_data,
	.arrayMemberSize = sizeof(imagescan_data_t),
	.parameterOffset = offsetof(imagescan_data_t, shutter),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_imagescan_focus = {
	.baseAddr = (uint8_t*)imageScan_data,
	.arrayMemberSize = sizeof(imagescan_data_t),
	.parameterOffset = offsetof(imagescan_data_t, focus),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_imagescan_rotf = {
	.baseAddr = (uint8_t*)imageScan_data,
	.arrayMemberSize = sizeof(imagescan_data_t),
	.parameterOffset = offsetof(imagescan_data_t, rotf),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_imagescan_speed = {
	.baseAddr = (uint8_t*)imageScan_data,
	.arrayMemberSize = sizeof(imagescan_data_t),
	.parameterOffset = offsetof(imagescan_data_t, speed),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_imagescan_rot = {
	.baseAddr = (uint8_t*)imageScan_data,
	.arrayMemberSize = sizeof(imagescan_data_t),
	.parameterOffset = offsetof(imagescan_data_t, rot),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_cameotri_red = {
	.baseAddr = (uint8_t*)cameotri_data,
	.arrayMemberSize = sizeof(cameotri_data_t),
	.parameterOffset = offsetof(cameotri_data_t, red),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_cameotri_green = {
	.baseAddr = (uint8_t*)cameotri_data,
	.arrayMemberSize = sizeof(cameotri_data_t),
	.parameterOffset = offsetof(cameotri_data_t, green),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_cameotri_blue = {
	.baseAddr = (uint8_t*)cameotri_data,
	.arrayMemberSize = sizeof(cameotri_data_t),
	.parameterOffset = offsetof(cameotri_data_t, blue),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_cameotri_dimmer = {
	.baseAddr = (uint8_t*)cameotri_data,
	.arrayMemberSize = sizeof(cameotri_data_t),
	.parameterOffset = offsetof(cameotri_data_t, dimmer),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_cameotri_strobe = {
	.baseAddr = (uint8_t*)cameotri_data,
	.arrayMemberSize = sizeof(cameotri_data_t),
	.parameterOffset = offsetof(cameotri_data_t, strobe),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_cameotri_color = {
	.baseAddr = (uint8_t*)cameotri_data,
	.arrayMemberSize = sizeof(cameotri_data_t),
	.parameterOffset = offsetof(cameotri_data_t, color),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_cameoflood_red = {
	.baseAddr = (uint8_t*)cameoflood_data,
	.arrayMemberSize = sizeof(cameoflood_data_t),
	.parameterOffset = offsetof(cameoflood_data_t, red),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_cameoflood_green = {
	.baseAddr = (uint8_t*)cameoflood_data,
	.arrayMemberSize = sizeof(cameoflood_data_t),
	.parameterOffset = offsetof(cameoflood_data_t, green),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_cameoflood_blue = {
	.baseAddr = (uint8_t*)cameoflood_data,
	.arrayMemberSize = sizeof(cameoflood_data_t),
	.parameterOffset = offsetof(cameoflood_data_t, blue),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_cameoflood_white = {
	.baseAddr = (uint8_t*)cameoflood_data,
	.arrayMemberSize = sizeof(cameoflood_data_t),
	.parameterOffset = offsetof(cameoflood_data_t, white),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_cameoflood_amber = {
	.baseAddr = (uint8_t*)cameoflood_data,
	.arrayMemberSize = sizeof(cameoflood_data_t),
	.parameterOffset = offsetof(cameoflood_data_t, amber),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_cameoflood_uv = {
	.baseAddr = (uint8_t*)cameoflood_data,
	.arrayMemberSize = sizeof(cameoflood_data_t),
	.parameterOffset = offsetof(cameoflood_data_t, uv),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_cameoflood_dimmer = {
	.baseAddr = (uint8_t*)cameoflood_data,
	.arrayMemberSize = sizeof(cameoflood_data_t),
	.parameterOffset = offsetof(cameoflood_data_t, dimmer),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_cameoflood_strobe = {
	.baseAddr = (uint8_t*)cameoflood_data,
	.arrayMemberSize = sizeof(cameoflood_data_t),
	.parameterOffset = offsetof(cameoflood_data_t, strobe),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_cameoflood_duration = {
	.baseAddr = (uint8_t*)cameoflood_data,
	.arrayMemberSize = sizeof(cameoflood_data_t),
	.parameterOffset = offsetof(cameoflood_data_t, duration),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_citycolor_speed = {
	.baseAddr = (uint8_t*)citycolor_data,
	.arrayMemberSize = sizeof(citycolor_data_t),
	.parameterOffset = offsetof(citycolor_data_t, motorspeed),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_citycolor_cyan = {
	.baseAddr = (uint8_t*)citycolor_data,
	.arrayMemberSize = sizeof(citycolor_data_t),
	.parameterOffset = offsetof(citycolor_data_t, cyan),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_citycolor_magenta = {
	.baseAddr = (uint8_t*)citycolor_data,
	.arrayMemberSize = sizeof(citycolor_data_t),
	.parameterOffset = offsetof(citycolor_data_t, magenta),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_citycolor_yellow = {
	.baseAddr = (uint8_t*)citycolor_data,
	.arrayMemberSize = sizeof(citycolor_data_t),
	.parameterOffset = offsetof(citycolor_data_t, yellow),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_citycolor_basiccolor = {
	.baseAddr = (uint8_t*)citycolor_data,
	.arrayMemberSize = sizeof(citycolor_data_t),
	.parameterOffset = offsetof(citycolor_data_t, basiccolors),
	.parameter_type = type_uint8
};

const  fixture_parameter_t param_citycolor_lamp = {
	.baseAddr = (uint8_t*)citycolor_data,
	.arrayMemberSize = sizeof(citycolor_data_t),
	.parameterOffset = offsetof(citycolor_data_t, lamp),
	.parameter_type = type_uint8
};

//const  fixture_parameter_t param_cameo_r = {
//	.baseAddr = (uint8_t*)imageScan_data,
//	.arrayMemberSize = sizeof(imagescan_data_t),
//	.parameterOffset = offsetof(imagescan_data_t, rot),
//	.parameter_type = type_uint8
//};

const fixture_parameterset_t fixture_parameterset[] =
{
	{ //imageScan
		.name = NAME_IMAGESCAN,
		.fname =  {			PARAM_SHUTTER, PARAM_FOCUS, PARAM_ROTF, PARAM_ROT, PARAM_SPEED, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
		.parameterConfig = { &param_imagescan_shutter, &param_imagescan_focus, &param_imagescan_rotf, &param_imagescan_rot, &param_imagescan_speed, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
	},
	{ //cameo tri 
		.name = NAME_CAMEO_TRI,
		.fname = { PARAM_R, PARAM_G, PARAM_B, PARAM_DIMMER, PARAM_STROBE, PARAM_BASICCOLOR, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
		.parameterConfig = { &param_cameotri_red, &param_cameotri_green, &param_cameotri_blue, &param_cameotri_dimmer, &param_cameotri_strobe, &param_cameotri_color, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
	},
	{ //cameo flood 
		.name = NAME_CAMEO_FLOOD,
		.fname = { PARAM_R, PARAM_G, PARAM_B, PARAM_WHITE, PARAM_A, PARAM_UV, PARAM_DIMMER, PARAM_STROBE, PARAM_DURATION, NULL, NULL, NULL, NULL, NULL },
		.parameterConfig = { &param_cameoflood_red, &param_cameoflood_green, &param_cameoflood_blue, &param_cameoflood_white,&param_cameoflood_amber,&param_cameoflood_uv,&param_cameoflood_dimmer, &param_cameoflood_strobe, &param_cameoflood_duration, NULL, NULL, NULL, NULL, NULL }
		//.rotConfig = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
	},
	{ //City color
		.name = NAME_CITYCOLOR,
		.fname = { PARAM_SPEED, PARAM_CYAN, PARAM_MAGENT, PARAM_YELLOW, PARAM_BASICCOLOR, PARAM_LAMP, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
		.parameterConfig = { &param_citycolor_speed, &param_citycolor_cyan, &param_citycolor_magenta, &param_citycolor_yellow, &param_citycolor_basiccolor, &param_citycolor_lamp, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
		//.rotConfig = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
	},
};



void main_init(void)
{
	main_show();
	parameterSetIndex = imageScan;
}

void main_show(void)
{
	updateConfig();
	updateLcd();

}	


void main_run(void)
{
	fixturekey_t fixsel = get_fixturekeys();
	static uint8_t lastPress = 0;
	if(fixsel != FIX_NO_KEY)
	{
		if(!lastPress)
		{
			switch(fixsel)
			{
				case FIX1: previousFixture(); break;
				case FIX2: nextFixture(); break;
				case FIX3: select_pressed();break;
				case FIX4: deselectAll_pressed(); break;
			}

			//set_fixture_leds(fixture_selected);
			updateConfig();
			updateLcd();
			updateSelectionLEDs();
		}
		lastPress = 1;
	}
	else
	{
		lastPress = 0;
	}
	

    fselkey_t fsel = get_fselkeys();

	static uint8_t lastBank=0;

    if(fsel != FSEL_NO_KEY)
	{
		switch(fsel)
		{
			case FSEL1: if( fixture_parameterset[parameterSetIndex].fname[0+parameterBank*7]) active_param = 0 + parameterBank*7;break;
			case FSEL2: if( fixture_parameterset[parameterSetIndex].fname[1+parameterBank*7]) active_param = 1 + parameterBank*7;break;
			case FSEL3: if( fixture_parameterset[parameterSetIndex].fname[2+parameterBank*7]) active_param = 2 + parameterBank*7;break;
			case FSEL4: if( fixture_parameterset[parameterSetIndex].fname[3+parameterBank*7]) active_param = 3 + parameterBank*7;break;
			case FSEL5: if( fixture_parameterset[parameterSetIndex].fname[4+parameterBank*7]) active_param = 4 + parameterBank*7;break;
			case FSEL6: if( fixture_parameterset[parameterSetIndex].fname[5+parameterBank*7]) active_param = 5 + parameterBank*7;break;
			case FSEL7: if( fixture_parameterset[parameterSetIndex].fname[6+parameterBank*7]) active_param = 6 + parameterBank*7;break;
			case FSEL8: if(lastBank==0) { lastBank =1; parameterBank = parameterBank==1?0:1; } break;
		}
		set_fsel_leds(active_param, parameterBank);
		set_bank_led(parameterBank);
		updateConfig();
		updateLcd();
	}
	else
	{
		lastBank=0;
	}
	


	uint8_t activeImageScan =getCurrentIndex();
	int16_t diff = axis_offset(0, 2);
	uint8_t lcdUpdate = 0;

	if(diff!=0)
		lcdUpdate=1;
	if( (int32_t)imageScan_data[activeImageScan].pan+diff > 65535)
			imageScan_data[activeImageScan].pan=65535;
	else if( (int32_t)imageScan_data[activeImageScan].pan+diff < 0)
			imageScan_data[activeImageScan].pan=0;
	else
			imageScan_data[activeImageScan].pan+=diff      ;

	//fetch joystick deflection and calculate new tilt
	diff = -axis_offset(1, 2);
	if(diff!=0)
		lcdUpdate=1;

	if( (int32_t)imageScan_data[activeImageScan].tilt+diff > 65535)
			imageScan_data[activeImageScan].tilt=65535;
	else if( (int32_t)imageScan_data[activeImageScan].tilt+diff < 0)
			imageScan_data[activeImageScan].tilt=0;
	else
			imageScan_data[activeImageScan].tilt+=diff     ;

	if(lcdUpdate)
 	   updateLcd();

}

static void updateParameter(void)
{
	
	switch(parameterSetIndex)
	{
		case imageScan: 
			if(imageScan_data[getCurrentIndex()].selected)
				for(uint8_t i=0; i < MOVING_FIXTURE_COUNT; i++)
				{
					if(imageScan_data[i].selected)
					{
						*get_param_u8_addr(i) = *get_param_u8_addr(getCurrentIndex());
					}
				}
		break;
		case cameo_tri:
			if(cameotri_data[getCurrentIndex()].selected) 
				for(uint8_t i=0; i < CAMEOTRI_FIXTURE_COUNT; i++)
				{
					if(cameotri_data[i].selected)
					{
						*get_param_u8_addr(i) = *get_param_u8_addr(getCurrentIndex());

					}
				}
		break;
		case cameo_flood: 
			if(cameoflood_data[getCurrentIndex()].selected) 
				for(uint8_t i=0; i < CAMEOFLOOD_FIXTURE_COUNT; i++)
				{
					if(cameoflood_data[i].selected)
					{
						*get_param_u8_addr(i) = *get_param_u8_addr(getCurrentIndex());

					}
				}
		break;
		case citycolor: 
			if(citycolor_data[getCurrentIndex()].selected) 
				for(uint8_t i=0; i < CITYCOLOR_FIXTURE_COUNT; i++)
				{
					if(citycolor_data[i].selected)
					{
						*get_param_u8_addr(i) = *get_param_u8_addr(getCurrentIndex());

					}
				}
		break;

	}

	updateLcd();
}


static void updateConfig(void)
{
	static rotary_config_t rotconfig = {
		.data_u8 = NULL,
		.type = rotary_uint8,
		.min = 0,
		.max = 255,
		.change = updateParameter,
		.multi = 1,
		.wrap = 0,
		.leds_on = 1
	};

	rotconfig.data_u8 = get_param_u8_addr(getCurrentIndex());
	
	rotary_setconfig(&rotconfig);
}

static void updateLcd(void)
{
	lcd_gotoxy(0,0);
	lcd_puts("                    ");
	lcd_gotoxy(0,0);
	lcd_puts_p(fixture_parameterset[parameterSetIndex].name);
	char tmp[20];
	sprintf(tmp," %d",getCurrentIndex()+1);
	lcd_puts(tmp);

	lcd_gotoxy(0,1);
	lcd_puts("                    ");
	lcd_gotoxy(0,1);
	if(fixture_parameterset[parameterSetIndex].fname[active_param])
		lcd_puts_p(fixture_parameterset[parameterSetIndex].fname[active_param]);



	if(fixture_parameterset[parameterSetIndex].parameterConfig[active_param])
	{
		if(fixture_parameterset[parameterSetIndex].parameterConfig[active_param]->parameter_type== type_uint8)
		{
			uint8_t* data = get_param_u8_addr(getCurrentIndex());
			sprintf(tmp, "  %3d",*data);
		}
		lcd_puts(tmp);
	}

	if(parameterSetIndex == imageScan)
	{
		lcd_gotoxy(0,2);
		sprintf(tmp, "%u:%u        ",imageScan_data[0].pan,imageScan_data[0].tilt);
		lcd_puts(tmp);
	}


	lcd_gotoxy(15,0);
	sprintf(tmp, "%d %d",imageScan_data[0].selected,imageScan_data[1].selected);
	lcd_puts(tmp);
	lcd_gotoxy(19,1);
//	
//	sprintf(tmp, "%d",active_param);
//	lcd_puts(tmp);
}


uint8_t* get_param_u8_addr (uint8_t index)
{
	return (uint8_t*)fixture_parameterset[parameterSetIndex].parameterConfig[active_param]->baseAddr + 
				fixture_parameterset[parameterSetIndex].parameterConfig[active_param]->arrayMemberSize * index +
				fixture_parameterset[parameterSetIndex].parameterConfig[active_param]->parameterOffset;
}


uint8_t getCurrentIndex(void)
{
	if(fixture_selected >= 0 && fixture_selected < MOVING_FIXTURE_COUNT)
		return fixture_selected;
	else if (fixture_selected < CAMEOTRI_FIXTURE_COUNT+MOVING_FIXTURE_COUNT)
		return fixture_selected-MOVING_FIXTURE_COUNT;
	else if (fixture_selected < MOVING_FIXTURE_COUNT + CAMEOTRI_FIXTURE_COUNT + CAMEOFLOOD_FIXTURE_COUNT)
		return fixture_selected-MOVING_FIXTURE_COUNT-CAMEOTRI_FIXTURE_COUNT;
	else
		return fixture_selected-MOVING_FIXTURE_COUNT-CAMEOTRI_FIXTURE_COUNT-CAMEOFLOOD_FIXTURE_COUNT;
	
}

void nextFixture(void)
{
	fixture_selected++; 
	
	if(fixture_selected >= TOTAL_FIXTURE_COUNT)
		fixture_selected = 0; 


	if(fixture_selected < MOVING_FIXTURE_COUNT) 
		parameterSetIndex = imageScan; 
	else if (fixture_selected < MOVING_FIXTURE_COUNT + CAMEOTRI_FIXTURE_COUNT)
		parameterSetIndex = cameo_tri;
	else if (fixture_selected < MOVING_FIXTURE_COUNT + CAMEOTRI_FIXTURE_COUNT + CAMEOFLOOD_FIXTURE_COUNT)
		parameterSetIndex = cameo_flood;
	else
		parameterSetIndex = citycolor;

}

void previousFixture(void)
{
	fixture_selected--; 
	
	if(fixture_selected >=TOTAL_FIXTURE_COUNT)
		fixture_selected = TOTAL_FIXTURE_COUNT-1;


	if(fixture_selected < MOVING_FIXTURE_COUNT) 
		parameterSetIndex = imageScan; 
	else if (fixture_selected < MOVING_FIXTURE_COUNT + CAMEOTRI_FIXTURE_COUNT)
		parameterSetIndex = cameo_tri;
	else if (fixture_selected < MOVING_FIXTURE_COUNT + CAMEOTRI_FIXTURE_COUNT + CAMEOFLOOD_FIXTURE_COUNT)
		parameterSetIndex = cameo_flood;
	else
		parameterSetIndex = citycolor;
	


}

static void updateSelectionLEDs(void)
{
	uint8_t on;
	switch(parameterSetIndex)
	{
		case imageScan: 
			set_selecte_led(imageScan_data[getCurrentIndex()].selected);
			on=0;
			for(uint8_t i=0; i < MOVING_FIXTURE_COUNT; i++)
			{
				if(imageScan_data[i].selected)
					on=1;
			}
			set_deselectAll_led(on);
		break;
		case cameo_tri: 
			set_selecte_led(cameotri_data[getCurrentIndex()].selected);
			on=0;
			for(uint8_t i=0; i < CAMEOTRI_FIXTURE_COUNT; i++)
			{
				if(cameotri_data[i].selected)
					on=1;
			}
			set_deselectAll_led(on);
		break;
		case cameo_flood: 
			set_selecte_led(cameoflood_data[getCurrentIndex()].selected);
			on=0;
			for(uint8_t i=0; i < CAMEOFLOOD_FIXTURE_COUNT; i++)
			{
				if(cameoflood_data[i].selected)
					on=1;
			}
			set_deselectAll_led(on);
		break;
		case citycolor: 
			set_selecte_led(citycolor_data[getCurrentIndex()].selected);
			on=0;
			for(uint8_t i=0; i < CITYCOLOR_FIXTURE_COUNT; i++)
			{
				if(citycolor_data[i].selected)
					on=1;
			}
			set_deselectAll_led(on);
		break;
	}
}

static void select_pressed(void)
{
	if(parameterSetIndex==imageScan)
	{
		selected_status=selected_imagescan;
		if(imageScan_data[getCurrentIndex()].selected==0)
		{
			imageScan_data[getCurrentIndex()].selected=1;
		}
		else
		{
			imageScan_data[getCurrentIndex()].selected=0;
		}
	}
	else if(parameterSetIndex==cameo_tri)
	{
		selected_status=selected_tri;
		if(cameotri_data[getCurrentIndex()].selected==0)
		{
			cameotri_data[getCurrentIndex()].selected=1;
		}
		else
		{
			cameotri_data[getCurrentIndex()].selected=0;
		}
	}
	else if(parameterSetIndex==cameo_flood)
	{
		selected_status=selected_flood;
		if(cameoflood_data[getCurrentIndex()].selected==0)
		{
			cameoflood_data[getCurrentIndex()].selected=1;
		}
		else
		{
			cameoflood_data[getCurrentIndex()].selected=0;
		}
	}
	else if(parameterSetIndex==citycolor)
	{
		selected_status=selected_citycolor;
		if(citycolor_data[getCurrentIndex()].selected==0)
		{
			citycolor_data[getCurrentIndex()].selected=1;
		}
		else
		{
			citycolor_data[getCurrentIndex()].selected=0;
		}
	}

	updateSelectionLEDs();
}

static void deselectAll_pressed(void)
{
	switch(parameterSetIndex)
	{
		case imageScan: 
			for(uint8_t i=0; i < MOVING_FIXTURE_COUNT; i++)
			{
				imageScan_data[i].selected=0;
			}
		break;
		case cameo_tri: 
			for(uint8_t i=0; i < CAMEOTRI_FIXTURE_COUNT; i++)
			{
				cameotri_data[i].selected=0;
			}
		break;
		case cameo_flood: 
			for(uint8_t i=0; i < CAMEOFLOOD_FIXTURE_COUNT; i++)
			{
				cameoflood_data[i].selected=0;
			}
		break;
		case citycolor: 
			for(uint8_t i=0; i < CITYCOLOR_FIXTURE_COUNT; i++)
			{
				citycolor_data[i].selected=0;
			}
		break;
		

	}

	updateSelectionLEDs();
}
