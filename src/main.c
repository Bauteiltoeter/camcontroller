#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <util/delay.h>
#include "lcd.h"
#include "dmx.h"
#include "adc.h"
#include "hardware.h"



typedef enum {
	MENU_SPLASH =0,
	MENU_MAIN = 1,
	MENU_SETUP = 2,
	MENU_EDIT_CAM = 3,
	MENU_STORE = 4,
	MENU_CLEAR = 5,
	MENU_INVALID = 255
} menu_identifiers;

typedef void(*menu_button)(void);
typedef void(*init_function)(void);

// This struct defines a menu page
// Each page contains the 4 LCD lines, the ID of the reachable menues, callbacks for each buttons and an init-function called when opening the menu
//
typedef struct
{
	char* lines[4];				//Display content
	menu_identifiers next[4];	//Menu to reach if a softkey is pressed
	menu_button cb[4];			//Callback for buttonpresses
	init_function init;			//Init function
	
} menu_t;

//Struct for cam data
#define PARAM_COUNT 5
#define CAM_COUNT 4
#define STORE_COUNT 4
typedef struct  {
	uint16_t base_addr;		//DMX base address
	uint16_t pan_address;	//Offset of pan address from base
	uint16_t tilt_address;	//Offset of tilt address from base
	uint16_t pan_invert;	//Invert pan? (0/1)
	uint16_t tilt_invert;	//Invert tilt? (0/1)
	uint8_t pan;			//Pan value (send over DMX)
	uint8_t tilt;			//Tilt value (send over DMX)

	uint16_t store_pan[STORE_COUNT];	//Stored positions
	uint16_t store_tilt[STORE_COUNT];
} cam_data_t;

void setup_cam_up(void);
void setup_cam_down(void);
void setup_show_cam(void);
void param_next(void);
void param_up(void);
void param_down(void);
void param_show(void);
void param_resetId(void);
void main_show(void);
void store_clear(void);
void save_data(void);
void set_menu(menu_identifiers menu);
void process_menu(void);
void process_inputs(void);


//###########################
//  Global variables
//###########################
menu_identifiers active_menu;
cam_data_t cams[CAM_COUNT];	//Data for each cam
cam_data_t backup_cams[CAM_COUNT] EEMEM; //store CAM Data in eeprom
uint8_t active_cam; //remember the active cam

//Define the menu structure
menu_t menues[] =
{ 	
	{ //MENU_SPLASH
		.lines = { "DragonVideo        ","By Karrn            ","Cam controller      ","                2017"},
		.next =  { MENU_MAIN,MENU_MAIN,MENU_MAIN,MENU_MAIN},
		.cb =    { NULL,NULL,NULL,NULL},
		.init =  NULL
    },
	{ //MENU_MAIN
		.lines = { "DragonVideo        ","                    ","                    ","STORE          SETUP"},
		.next  = { MENU_STORE, MENU_INVALID,MENU_INVALID,MENU_SETUP},
		.cb    = { NULL,NULL,NULL,NULL},
		.init  = main_show
	},
	{ //MENU_SETUP
		.lines = { "Setup              ","                    ","                    ","PREV NEXT MOD.  BACK"},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_EDIT_CAM,MENU_MAIN},
		.cb    = { setup_cam_down,setup_cam_up,param_resetId,save_data},
		.init =  setup_show_cam
	},
	{ //MENU_EDIT_CAM
		.lines = { "Setup              ","                    ","                    ","NEXT UP   DOWN  BACK"},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_SETUP},
		.cb    = { param_next,param_up, param_down, NULL},
		.init  = param_show
	},
	{ //MENU_STORE
		.lines = { "Store              ","Choose store to save","                    ","     CLEAR     ABORT"},
		.next  = { MENU_INVALID,MENU_CLEAR,MENU_INVALID,MENU_MAIN},
		.cb    = { NULL,NULL, NULL, NULL},
		.init  = NULL
	},
	{ //MENU_CLEAR
		.lines = { "Clear store        ","Choose store        ","                    ","ALL            ABORT"},
		.next  = { MENU_MAIN,MENU_INVALID,MENU_INVALID,MENU_MAIN},
		.cb    = { store_clear,NULL, NULL, NULL},
		.init  = NULL
	}
};





//Hauptprogramm
int main (void) 
{  
	//initialise stuff
	hardware_init();
	eeprom_read_block (cams, backup_cams, sizeof(cams));

	lcd_init(LCD_DISP_ON);
	set_menu(MENU_SPLASH);
	dmx_init();
	ADC_Init();

	uint16_t blink_counter=0;
	while(1)
	{
		blink_counter++;

		//Process analog inputs, cam changes and store requests
		process_inputs();
		
		//Process the menu system  / softkeys
		process_menu();
		

		//The STORE-LEDs should flash if MENU_STORE or MENU_CLEAR is selected
		if((active_menu == MENU_STORE || active_menu == MENU_CLEAR) && blink_counter % 20 == 0)
		{
			for(int i=0; i < STORE_COUNT; i++)
				reset_store_led(i);
		}

		if((active_menu == MENU_STORE || active_menu == MENU_CLEAR )&& blink_counter % 40 == 0)
		{
			for(int i=0; i < STORE_COUNT; i++)
				set_store_led(i);
		}


		//awkward delay here.. should be replaced by a timer. But it works, DMX is interrupt-driven
		_delay_ms(5);
	}
}




void process_inputs(void)
{
	//Fetch keypress from hardware
	camkey_t keys = get_camkeys();

	//Change the active cam
	switch(keys)
	{
		case CAM1: active_cam = 0; break;
		case CAM2: active_cam = 1; break; 
		case CAM3: active_cam = 2; break;
		case CAM4: active_cam = 3; break;
		default: break;
	}

	//update stuff if the cam changed
	if(keys != CAM_NO_KEY)
	{
		//camera leds
		set_cam_leds(active_cam);

		//update the store leds if the leds are not flashing
		if( ! (active_menu == MENU_STORE || active_menu == MENU_CLEAR))
		{
			for(int i=0; i < STORE_COUNT; i++)
			{
				//0xFFFF marks an empty store
				if(cams[active_cam].store_pan[i]!= 0xFFFF && cams[active_cam].store_tilt[i]!= 0xFFFF)
				{
					set_store_led(i);
				}
				else
				{
					reset_store_led(i);
				}
			}
		}

		//main screen
		if(active_menu == MENU_MAIN)
			main_show();

	}
	
	//fetch joystick deflection and calculate new pan
	int16_t diff = axis_offset(0);
	if(cams[active_cam].pan_invert)
	{
		diff=-diff;
	}
	uint8_t old_pan = cams[active_cam].pan;
	if( (int16_t)cams[active_cam].pan+diff > 255)
		cams[active_cam].pan=255;
	else if( (int16_t)cams[active_cam].pan+diff < 0)
		cams[active_cam].pan=0;
	else
		cams[active_cam].pan+=diff	;

	//fetch joystick deflection and calculate new tilt
	diff = -axis_offset(1);
	if(cams[active_cam].tilt_invert)
	{
		diff=-diff;
	}

	uint8_t old_tilt = cams[active_cam].tilt;
	if( (int16_t)cams[active_cam].tilt+diff > 255)
		cams[active_cam].tilt=255;
	else if( (int16_t)cams[active_cam].tilt+diff < 0)
		cams[active_cam].tilt=0;
	else
		cams[active_cam].tilt+=diff	;

	//fetch store key selection from hardware
	storekey_t store = get_storekeys();

	if(store != STORE_NO_KEY)
	{
		uint8_t store_id=0;

		//get store id from button
		switch(store)
		{
			case STORE1: store_id = 0; break;
			case STORE2: store_id = 1; break;
			case STORE3: store_id = 2; break;
			case STORE4: store_id = 3; break;
			default: break;
		}

		//if the current menu is store, save the data, else read or clear
		if(active_menu == MENU_STORE)
		{
			cams[active_cam].store_pan[store_id] = cams[active_cam].pan;
			cams[active_cam].store_tilt[store_id] = cams[active_cam].tilt;
			save_data(); //write data to EEPROM
			set_menu(MENU_MAIN); //jump back to main menu
		}
		else if(active_menu == MENU_CLEAR) //if the current menu is clear, clear the selected slot
		{
			cams[active_cam].store_pan[store_id]=0xFFFF;
			cams[active_cam].store_tilt[store_id]=0xFFFF;
			save_data(); //write data to EEPROM
			set_menu(MENU_MAIN); //jump back to main menu
		}
		else //load stored values
		{
			//can not load empty store
			if(cams[active_cam].store_pan[store_id]!= 0xFFFF &&  cams[active_cam].store_tilt[store_id] != 0xFFFF)
			{
				cams[active_cam].pan = cams[active_cam].store_pan[store_id]; 
				cams[active_cam].tilt = cams[active_cam].store_tilt[store_id];;
			}
		}

	}

	//if tilt or pan changed
	if(old_tilt != cams[active_cam].tilt || old_pan != cams[active_cam].pan)
	{
		//write values to DMX
		write_dmx(cams[active_cam].base_addr + cams[active_cam].pan_address, cams[active_cam].pan);
		write_dmx(cams[active_cam].base_addr + cams[active_cam].tilt_address, cams[active_cam].tilt);

		//update main menu
		if(active_menu == MENU_MAIN)
			main_show();
	}	

}

void store_clear(void)
{
	for(uint8_t i=0; i < STORE_COUNT; i++)
	{
		cams[active_cam].store_pan[i] = 0xFFFF;
		cams[active_cam].store_tilt[i] = 0xFFFF;
	}
}

void main_show(void)
{
	lcd_gotoxy(0,1);
	lcd_puts("Active: CAM ");
	char tmp[21];
	itoa(active_cam+1,tmp,10);
	lcd_gotoxy(12,1);
	lcd_puts(tmp);

	lcd_gotoxy(0,2);
	sprintf(tmp,"Pan: %3d Tilt: %3d", cams[active_cam].pan, cams[active_cam].tilt);
	lcd_puts(tmp);
}	

void process_menu(void)
{
	softkey_t button = get_softkeys();

	if(button != NO_KEY )
	{
		if(menues[active_menu].cb[button] != NULL)
				menues[active_menu].cb[button]();	

		if(menues[active_menu].next[button] != MENU_INVALID)
			set_menu(menues[active_menu].next[button]);
	
					

	}	
}

void set_menu(menu_identifiers menu)
{
	active_menu = menu;
	lcd_clrscr();
	lcd_puts(menues[menu].lines[0]);
	lcd_gotoxy(0,1);
	lcd_puts(menues[menu].lines[1]);
	lcd_gotoxy(0,2);
	lcd_puts(menues[menu].lines[2]);
	lcd_gotoxy(0,3);
	lcd_puts(menues[menu].lines[3]);

	if(menues[menu].init)
		menues[menu].init();


}

uint8_t setup_active_cam=0;
uint8_t param_id=0;
uint16_t* changing_param;

void setup_show_cam(void)
{
	lcd_gotoxy(0,1);

	char tmp[10];
	itoa(setup_active_cam+1,tmp,10);
	lcd_puts("Edit Cam ");
	lcd_gotoxy(9,1);
	lcd_puts(tmp);
}

void setup_cam_up(void)
{
	setup_active_cam++;

	if(setup_active_cam>= CAM_COUNT)
		setup_active_cam=0;

	setup_show_cam();
}

void setup_cam_down(void)
{
	setup_active_cam--;
	if(setup_active_cam>= CAM_COUNT)
		setup_active_cam= CAM_COUNT-1;

	setup_show_cam();
}

void param_next(void)
{
	param_id++;

	if(param_id>=PARAM_COUNT)
		param_id=0;

	param_show();
}

void param_up(void)
{
	(*changing_param)++;
	param_show();	
}

void param_down(void)
{
	(*changing_param)--;
	param_show();	
}

void param_show(void)
{
	lcd_gotoxy(14,0);
	lcd_puts("CAM ");
	char tmp[10];
	itoa(setup_active_cam+1,tmp,10);
	lcd_gotoxy(18,0);
	lcd_puts(tmp);


	lcd_gotoxy(0,1);
	char toDraw[20];
	switch(param_id)
	{
		case 0: lcd_puts("base_addr           "); 
				changing_param=&cams[setup_active_cam].base_addr;
		
				break;
		case 1: lcd_puts("pan offset          "); 
				changing_param=&cams[setup_active_cam].pan_address;
		
				break;
		case 2: lcd_puts("tilt offset         "); 
				changing_param=&cams[setup_active_cam].tilt_address;
				break;
		case 3: lcd_puts("invert pan          "); 
				changing_param=&cams[setup_active_cam].pan_invert;
				break;
		case 4: lcd_puts("invert tilt         "); 
				changing_param=&cams[setup_active_cam].tilt_invert;
				break;
		
	}

	itoa(*changing_param,toDraw,10);
	lcd_gotoxy(0,2);
	lcd_puts("                 ");
	lcd_gotoxy(0,2);
	lcd_puts(toDraw);
}

void param_resetId(void)
{
	param_id=0;
}

void save_data(void)
{
	eeprom_write_block (cams, backup_cams, sizeof(cams));

}






