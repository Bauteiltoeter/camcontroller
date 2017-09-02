#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <util/delay.h>
	#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include "lcd.h"
#include "dmx.h"
#include "adc.h"
#include "hardware.h"
#include "rotary.h"


typedef enum {
	MENU_SPLASH =0,
	MENU_MAIN = 1,
	MENU_SETUP = 2,
	MENU_EDIT_CAM = 3,
	MENU_STORE = 4,
	MENU_CLEAR = 5,
	MENU_GENERAL_SETUP = 6,
	MENU_RESET = 7,
	MENU_CTRL = 8,
	MENU_CTRL_EDIT = 9,
	MENU_LOCKED = 10,
	MENU_LOCK_SETUP = 11,
	MENU_INVALID = 255
} menu_identifiers;

typedef void(*menu_button)(void);
typedef void(*init_function)(void);

// This struct defines a menu page
// Each page contains the 4 LCD lines, the ID of the reachable menues, callbacks for each buttons and an init-function called when opening the menu
//
typedef struct
{
	const char* lines[4];				//Display content
	menu_identifiers next[4];	//Menu to reach if a softkey is pressed
	menu_button cb[4];			//Callback for buttonpresses
	menu_button cb_r[4];
	init_function init;			//Init function
	
} menu_t;


#define CAM_COUNT 4
#define STORE_COUNT 4
typedef struct  {
	uint16_t base_addr;		//DMX base address
	uint16_t pan_address;	//Offset of pan address from base
	uint16_t tilt_address;	//Offset of tilt address from base
	uint16_t speed_address;
	uint16_t switch_address;
	uint8_t pan_invert;	//Invert pan? (0/1)
	uint8_t tilt_invert;	//Invert tilt? (0/1)
	uint16_t pan_scaling;  
	uint16_t tilt_scaling;
	uint8_t pan;			//Pan value (send over DMX)
	uint8_t tilt;			//Tilt value (send over DMX)
	uint8_t speed;
	uint8_t button_state;
	uint8_t power_state;
	

	uint16_t store_pan[STORE_COUNT];	//Stored positions
	uint16_t store_tilt[STORE_COUNT];
} cam_data_t;

typedef enum {
	type_uint16,
	type_uint8
} parameter_type;

typedef struct {
	char* name;
	parameter_type type;
	size_t offset;
	uint16_t min;
	uint16_t max;
} cam_setup_parameters_t;


#define NUMBER_OF_CAM_PARAMETERS  (sizeof(cam_setup_parameters)/sizeof(cam_setup_parameters_t))
cam_setup_parameters_t cam_setup_parameters[] = 
{
	{
		.name = "base_addr           ",
		.type = type_uint16,
		.offset = offsetof(cam_data_t, base_addr),
		.min = 0,
		.max = 512
	},
	{
		.name = "pan address         ",
		.type = type_uint16,
		.offset = offsetof(cam_data_t, pan_address),
		.min = 0,
		.max = 512
	},
	{
		.name = "tilt address       ",
		.type = type_uint16,
		.offset = offsetof(cam_data_t, tilt_address),
		.min = 0,
		.max = 512
	},
	{
		.name = "speed address       ",
		.type = type_uint16,
		.offset = offsetof(cam_data_t, speed_address),
		.min = 0,
		.max = 512
	},
	{
		.name = "switch address      ",
		.type = type_uint16,
		.offset = offsetof(cam_data_t, switch_address),
		.min = 0,
		.max = 512
	},
	{
		.name = "invert pan          ",
		.type = type_uint8,
		.offset = offsetof(cam_data_t, pan_invert),
		.min = 0,
		.max = 1
	},
	{
		.name = "invert tilt         ",
		.type = type_uint8,
		.offset = offsetof(cam_data_t, tilt_invert),
		.min = 0,
		.max = 1
	},
	{
		.name = "pan scaling         ",
		.type = type_uint16,
		.offset = offsetof(cam_data_t, pan_scaling),
		.min = 0,
		.max = 100
	},
	{
		.name = "tilt scaling        ",
		.type = type_uint16,
		.offset = offsetof(cam_data_t, tilt_scaling),
		.min = 0,
		.max = 100
	}
};

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
void update_leds(void);
void load_default(void);

void ctrl_cam_up(void);
void ctrl_cam_down(void);
void ctrl_cam_show(void);

void cam_power_on(void);
void cam_power_off(void);
void cam_button(void);
void cam_power_show(void);

void send_switch(void);

void setup_menu_next(void);
void setup_menu_prev(void);
void setup_menu_enter(void);
void setup_menu_show(void);
void setup_reset(void);

void lock_1_pressed(void);
void lock_2_pressed(void);
void lock_3_pressed(void);
void lock_4_pressed(void);
void lock_pressed(uint8_t n);

//###########################
//  Global variables
//###########################
menu_identifiers active_menu;


cam_data_t cams_default[CAM_COUNT] = 
{
	{
		.base_addr = 0,
		.pan_address = 0,
		.tilt_address = 2,
		.speed_address = 4,
		.switch_address = 5,
		.pan_invert = 0,
		.tilt_invert=0,
		.pan_scaling = 30,
		.tilt_scaling = 20,
		.pan = 0, 
		.tilt = 0,
		.speed = 0,
		.store_pan = { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF},
		.store_tilt= { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF} 
	},	
	{
		.base_addr = 16,
		.pan_address = 0,
		.tilt_address = 2,
		.speed_address = 4,
		.switch_address = 5,
		.pan_invert = 0,
		.tilt_invert=0,
		.pan_scaling = 30,
		.tilt_scaling = 20,
		.pan = 0, 
		.tilt = 0,
		.speed = 0,
		.store_pan = { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF},
		.store_tilt= { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF} 
	},
	{
		.base_addr = 32,
		.pan_address = 0,
		.tilt_address = 2,
		.speed_address = 4,
		.switch_address = 5,
		.pan_invert = 0,
		.tilt_invert=0,
		.pan_scaling = 30,
		.tilt_scaling = 20,
		.pan = 0, 
		.tilt = 0,
		.speed = 0,
		.store_pan = { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF},
		.store_tilt= { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF} 
	},
	{
		.base_addr = 0xFFFF,
		.pan_address = 0xFFFF,
		.tilt_address = 0xFFFF,
		.speed_address = 0xFFFF,
		.switch_address = 0xFFFF,
		.pan_invert = 0xFFFF,
		.tilt_invert=0xFFFF,
		.pan_scaling = 0xFFFF,
		.tilt_scaling = 0xFFFF,
		.pan = 0xFFFF, 
		.tilt = 0xFFFF,
		.speed = 0xFFFF,
		.store_pan = { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF},
		.store_tilt= { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF} 
	} 
};
cam_data_t cams[CAM_COUNT];	//Data for each cam
cam_data_t backup_cams[CAM_COUNT] EEMEM; //store CAM Data in eeprom
uint8_t active_cam; //remember the active cam

uint8_t code[] = {1,1,1,1};
uint8_t code_eeprom[4] EEMEM;

//Define the menu structure
menu_t menues[] =
{ 	
	{ //MENU_SPLASH
		.lines = { "DragonVideo        ","By Karrn            ","Cam controller      ","                2017"},
		.next =  { MENU_MAIN,MENU_MAIN,MENU_MAIN,MENU_MAIN},
		.cb =    { NULL,NULL,NULL,NULL},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init =  NULL
    },
	{ //MENU_MAIN
		.lines = { "                   ","                    ","                    ","STORE          SETUP"},
		.next  = { MENU_STORE, MENU_INVALID,MENU_INVALID,MENU_GENERAL_SETUP},
		.cb    = { NULL,NULL,NULL,NULL},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = main_show
	},
	{ //MENU_SETUP
		.lines = { "Setup              ","                    ","                    ","PREV NEXT EDIT  BACK"},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_EDIT_CAM,MENU_GENERAL_SETUP},
		.cb    = { setup_cam_down,setup_cam_up,param_resetId,NULL},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init =  setup_show_cam
	},
	{ //MENU_EDIT_CAM
		.lines = { "Setup              ","                    ","                    ","NEXT UP   DOWN  BACK"},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_SETUP},
		.cb    = { param_next,param_up, param_down, NULL},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = param_show
	},
	{ //MENU_STORE
		.lines = { "Store              ","Choose store to save","                    ","     CLEAR     ABORT"},
		.next  = { MENU_INVALID,MENU_CLEAR,MENU_INVALID,MENU_MAIN},
		.cb    = { NULL,NULL, NULL, NULL},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = NULL
	},
	{ //MENU_CLEAR
		.lines = { "Clear store        ","Choose store        ","                    ","ALL            ABORT"},
		.next  = { MENU_MAIN,MENU_INVALID,MENU_INVALID,MENU_MAIN},
		.cb    = { store_clear,NULL, NULL, NULL},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = NULL
	}, 
	{ //MENU_GENERAL_SETUP
		.lines = { "Choose setup menu  ","                    ","                    ","PREV NEXT ENTER BACK"},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_MAIN},
		.cb    = { setup_menu_prev,setup_menu_next, setup_menu_enter, save_data},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = setup_reset
	}, 
	{ //MENU_RESET
		.lines = { "Setup              ","Load default config ","and erase all data? ","YES             BACK"},
		.next  = { MENU_MAIN,MENU_INVALID,MENU_INVALID,MENU_MAIN},
		.cb    = { load_default,NULL, NULL, NULL},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = NULL
	},  
	{ //MENU_CTRL
		.lines = { "Setup              ","                    ","                    ","PREV NEXT CTRL  BACK"},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_CTRL_EDIT,MENU_GENERAL_SETUP},
		.cb    = { ctrl_cam_down,ctrl_cam_up, NULL, NULL},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = ctrl_cam_show
	},  
	{ //MENU_CTRL_EDIT
		.lines = { "Setup              ","                    ","  Power             "," ON  OFF   BTN  BACK"},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_CTRL_EDIT,MENU_CTRL},
		.cb    = { cam_power_on,cam_power_off, cam_button, NULL},
		.cb_r=   { NULL,NULL,cam_button,NULL},
		.init  = cam_power_show
	} ,  
	{ //MENU_LOCKED
		.lines = { "    DragonVideo    ","  **** LOCKED ****  ","                    "," 1    2     3     4 "},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_INVALID},
		.cb    = { lock_1_pressed,lock_2_pressed, lock_3_pressed, lock_4_pressed},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = NULL
	} ,  
	{ //MENU_LOCK_SETUP
		.lines = { "Setup              ","Enter new code:     ","                    "," 1    2     3     4 "},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_INVALID},
		.cb    = {lock_1_pressed,lock_2_pressed, lock_3_pressed, lock_4_pressed},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = NULL
	}
};

typedef struct {
	menu_identifiers menu;
	char* description;

} setting_menu_t;

setting_menu_t setting_menues[] = {
	{
		.menu = MENU_SETUP,
		.description = "Camera setup        "
	},
	{
		.menu = MENU_CTRL,
		.description = "Camera controlling  "
	},
	{
		.menu = MENU_LOCKED,
		.description = "Lock cam controller "
	},
	{
		.menu = MENU_LOCK_SETUP,
		.description = "Enter lock code     "
	},
	{
		.menu = MENU_RESET,
		.description = "Reset settings      "
	}
};




//Hauptprogramm
int main (void) 
{  
	//initialise stuff
	hardware_init();
	eeprom_read_block (cams, backup_cams, sizeof(cams));
	eeprom_read_block (code, code_eeprom, sizeof(code));

	lcd_init(LCD_DISP_ON);
	set_menu(MENU_SPLASH);
	dmx_init();
	ADC_Init();
	rotary_init();

	send_switch();

	set_cam_leds(active_cam);
	uint16_t blink_counter=0;

	uint8_t loop=0;

	rotary_config_t test;

	test.type=rotary_uint8;
	test.data_u8 = &cams[0].pan;
	test.change = main_show;
	test.min = 10;
	test.max = 20;

	rotary_setconfig(&test);
	
	while(1)
	{
		blink_counter++;
		loop++;

		if(loop==10)
		{  
			if(active_menu != MENU_LOCKED)
			{
				//Process analog inputs, cam changes and store requests
				process_inputs();
			}

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
		
		

		

			rotary_process();
			loop=0;
		}
		
		rotary_tick();
		//awkward delay here.. should be replaced by a timer. But it works, DMX is interrupt-driven
		_delay_ms(0.5);
	}
}

void update_leds(void)
{
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

}



void process_inputs(void)
{
	//Fetch keypress from hardware
	camkey_t keys = get_camkeys();

	static rotary_config_t rotary_speed_config = 
		{
			.data_u8 = &cams[0].speed,
			.type = rotary_uint8,
			.min = 0,
			.max = 255,
			.change = main_show,
			.multi = 6
		};

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

		update_leds();
		rotary_speed_config.data_u8 = &cams[active_cam].speed;
		rotary_setconfig(&rotary_speed_config);

		//main screen
		if(active_menu == MENU_MAIN)
			main_show();

	}

	static uint8_t analog=0;

	
	
	uint8_t old_tilt = cams[active_cam].tilt;
	uint8_t old_pan = cams[active_cam].pan;
	
	analog++;
	if(analog%8==0)
	{
		//fetch joystick deflection and calculate new pan
		int16_t diff = axis_offset(0, cams[active_cam].pan_scaling);
		if(cams[active_cam].pan_invert)
		{
			diff=-diff;
		}
	
		if( (int16_t)cams[active_cam].pan+diff > 255)
			cams[active_cam].pan=255;
		else if( (int16_t)cams[active_cam].pan+diff < 0)
			cams[active_cam].pan=0;
		else
			cams[active_cam].pan+=diff	;

		//fetch joystick deflection and calculate new tilt
		diff = -axis_offset(1, cams[active_cam].tilt_scaling);
		if(cams[active_cam].tilt_invert)
		{
			diff=-diff;
		}


		if( (int16_t)cams[active_cam].tilt+diff > 255)
			cams[active_cam].tilt=255;
		else if( (int16_t)cams[active_cam].tilt+diff < 0)
			cams[active_cam].tilt=0;
		else
			cams[active_cam].tilt+=diff	;
	}

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
	
	//write values to DMX
	write_dmx(cams[active_cam].base_addr + cams[active_cam].pan_address, cams[active_cam].pan);
	write_dmx(cams[active_cam].base_addr + cams[active_cam].tilt_address, cams[active_cam].tilt);
	write_dmx(cams[active_cam].base_addr + cams[active_cam].speed_address, cams[active_cam].speed);

	//update main menu
	if(old_tilt != cams[active_cam].tilt || old_pan != cams[active_cam].pan)
	{
		if(active_menu == MENU_MAIN)
			main_show();
	}

	rotselkey_t rotsel = get_rotselkey();

	if(rotsel != ROTSEL_NO_KEY)
	{


		
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
	lcd_gotoxy(0,0);
	lcd_puts("Active: CAM ");
	char tmp[21];
	itoa(active_cam+1,tmp,10);
	lcd_gotoxy(12,0);
	lcd_puts(tmp);

	lcd_gotoxy(0,1);
	sprintf(tmp,"Pan: %3d Tilt: %3d", cams[active_cam].pan, cams[active_cam].tilt);
	lcd_puts(tmp);

	lcd_gotoxy(0,2);
	sprintf(tmp,"Speed: %3d", cams[active_cam].speed);
	lcd_puts(tmp);
}	

void process_menu(void)
{
	softkey_t button = get_softkeys();

	if(button != NO_KEY )
	{

		switch(button)
		{
			case SK1: 
			case SK2: 
			case SK3:
			case SK4: 
				if(menues[active_menu].cb[button] != NULL)
						menues[active_menu].cb[button]();	

				if(menues[active_menu].next[button] != MENU_INVALID)
					set_menu(menues[active_menu].next[button]);
			break;
			default:
				if(menues[active_menu].cb_r[button-4] != NULL)
						menues[active_menu].cb_r[button-4]();	

			break;
					
		}
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

	update_leds();

}

uint8_t setup_active_cam=0;
uint8_t param_id=0;


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

	if(param_id>=NUMBER_OF_CAM_PARAMETERS)
		param_id=0;

	param_show();
}

void param_up(void)
{
	switch(cam_setup_parameters[param_id].type)
	{
		case type_uint16:  (*(uint16_t*)(((void*)&cams[setup_active_cam]) + cam_setup_parameters[param_id].offset))++; 
						 if(*(uint16_t*)(((void*)&cams[setup_active_cam]) + cam_setup_parameters[param_id].offset)  > cam_setup_parameters[param_id].max )
							*(uint16_t*)(((void*)&cams[setup_active_cam]) + cam_setup_parameters[param_id].offset)  = cam_setup_parameters[param_id].min;
		break;
		case type_uint8:  (*(uint8_t*)(((void*)&cams[setup_active_cam]) + cam_setup_parameters[param_id].offset))++; 
					    if(*(uint8_t*)(((void*)&cams[setup_active_cam]) + cam_setup_parameters[param_id].offset)  > cam_setup_parameters[param_id].max )
						   *(uint8_t*)(((void*)&cams[setup_active_cam]) + cam_setup_parameters[param_id].offset)  = cam_setup_parameters[param_id].min;
		break;
	}
	param_show();	
}

void param_down(void)
{
	switch(cam_setup_parameters[param_id].type)
	{
		case type_uint16:  (*(uint16_t*)(((void*)&cams[setup_active_cam]) + cam_setup_parameters[param_id].offset))--; 
						 if(*(uint16_t*)(((void*)&cams[setup_active_cam]) + cam_setup_parameters[param_id].offset)  > cam_setup_parameters[param_id].max )
							*(uint16_t*)(((void*)&cams[setup_active_cam]) + cam_setup_parameters[param_id].offset)  = cam_setup_parameters[param_id].max;
		break;
		case type_uint8:  (*(uint8_t*)(((void*)&cams[setup_active_cam]) + cam_setup_parameters[param_id].offset))--; 
					    if(*(uint8_t*)(((void*)&cams[setup_active_cam]) + cam_setup_parameters[param_id].offset)  > cam_setup_parameters[param_id].max )
						   *(uint8_t*)(((void*)&cams[setup_active_cam]) + cam_setup_parameters[param_id].offset)  = cam_setup_parameters[param_id].max;
		break;
	}
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
	lcd_puts(cam_setup_parameters[param_id].name);

	switch(cam_setup_parameters[param_id].type)
	{
		case type_uint16: itoa( *(uint16_t*)(((void*)&cams[setup_active_cam]) + cam_setup_parameters[param_id].offset), tmp, 10); break;
		case type_uint8: itoa( *(uint8_t*)(((void*)&cams[setup_active_cam]) + cam_setup_parameters[param_id].offset), tmp, 10); break;
	}


	lcd_gotoxy(0,2);
	lcd_puts("                    ");
	lcd_gotoxy(0,2);
	lcd_puts(tmp);

}

void param_resetId(void)
{
	param_id=0;
}

void save_data(void)
{
	eeprom_write_block (cams, backup_cams, sizeof(cams));
	eeprom_write_block (code, code_eeprom, sizeof(code));
}

void load_default(void)
{
	memcpy(cams,cams_default, sizeof(cams)); 
	save_data();
}

uint8_t ctrl_selected_cam=0;


void ctrl_cam_up(void)
{
	ctrl_selected_cam++;

	if(ctrl_selected_cam>=CAM_COUNT)
	{
		ctrl_selected_cam=0;
	}

	ctrl_cam_show();

}

void ctrl_cam_down(void)
{
	ctrl_selected_cam--;

	if(ctrl_selected_cam>=CAM_COUNT)
	{
		ctrl_selected_cam=CAM_COUNT-1;
	}

	ctrl_cam_show();
}

void ctrl_cam_show(void)
{
	lcd_gotoxy(0,1);
	lcd_puts("Controll CAM ");
	
	lcd_gotoxy(13,1);
	char str[10];
	itoa(ctrl_selected_cam+1, str, 10);
	lcd_puts(str);
}

void cam_power_on(void)
{
	cams[ctrl_selected_cam].power_state=1;
	cam_power_show();
}

void cam_power_off(void)
{
	cams[ctrl_selected_cam].power_state=0;
	cam_power_show();
}

void cam_button(void)
{
	if(cams[ctrl_selected_cam].button_state)
	{
		cams[ctrl_selected_cam].button_state=0;
	}
	else
	{
	
		cams[ctrl_selected_cam].button_state=1;
	}

	cam_power_show();
}

void cam_power_show(void)
{
	if(cams[ctrl_selected_cam].power_state)
	{
		lcd_gotoxy(0,3);
		lcd_putc('>');
		lcd_gotoxy(3,3);
		lcd_putc('<');
		lcd_putc(' ');
		lcd_gotoxy(8,3);
		lcd_putc(' ');
	}
	else
	{
		lcd_gotoxy(0,3);
		lcd_putc(' ');
		lcd_gotoxy(3,3);
		lcd_putc(' ');
		lcd_putc('>');
		lcd_gotoxy(8,3);
		lcd_putc('<');
	}

	if(cams[ctrl_selected_cam].button_state)
	{
		cams[ctrl_selected_cam].button_state=1;
		lcd_gotoxy(10,3);
		lcd_putc('>');
		lcd_gotoxy(14,3);
		lcd_putc('<');

	}
	else
	{
		lcd_gotoxy(10,3);
		lcd_putc(' ');
		lcd_gotoxy(14,3);
		lcd_putc(' ');

	}

	send_switch();
}

void send_switch(void)
{
	uint8_t dmx;

	if(!cams[ctrl_selected_cam].button_state && !cams[ctrl_selected_cam].power_state)
		dmx=0;
	else if(!cams[ctrl_selected_cam].button_state && cams[ctrl_selected_cam].power_state)
		dmx=64;
	else if(cams[ctrl_selected_cam].button_state && !cams[ctrl_selected_cam].power_state)
		dmx=128;
	else if(cams[ctrl_selected_cam].button_state && cams[ctrl_selected_cam].power_state)
		dmx=255;

//	lcd_gotoxy(0,0);
	char rofl[20];

	itoa(dmx, rofl, 10);
//	lcd_puts(rofl);
//	lcd_puts("     ");

	write_dmx(cams[ctrl_selected_cam].base_addr + cams[ctrl_selected_cam].switch_address, dmx);
}

uint8_t current_menu=0;

void setup_menu_next(void)
{
	current_menu++;

	if(current_menu >= (sizeof(setting_menues)/sizeof(setting_menu_t)))
	{
		current_menu=0;
	}

	setup_menu_show();
}

void setup_menu_prev(void)
{
	current_menu--;

	if(current_menu >=(sizeof(setting_menues)/sizeof(setting_menu_t)))
	{
		current_menu=(sizeof(setting_menues)/sizeof(setting_menu_t))-1;
	}

	setup_menu_show();
}

void setup_menu_enter(void)
{
	set_menu(setting_menues[current_menu].menu);
}

void setup_menu_show(void)
{
	lcd_gotoxy(0,1);
	lcd_puts(setting_menues[current_menu].description);
}

void lock_1_pressed(void)
{
	lock_pressed(1);
}

void lock_2_pressed(void)
{
	lock_pressed(2);
}

void lock_3_pressed(void)
{
	lock_pressed(3);
}

void lock_4_pressed(void)
{
	lock_pressed(4);
}

void lock_pressed(uint8_t n)
{
	static uint8_t index=0;
	static uint8_t numbers[4];

	lcd_gotoxy(8+index,2);

	if(active_menu == MENU_LOCK_SETUP)
		lcd_putc('0'+n);
	else
		lcd_putc('*');

	numbers[index]=n;
	index++;
	
	if(index==4)
	{
		index=0;
	
		if(active_menu == MENU_LOCK_SETUP)
		{
			for(uint8_t i=0; i < 4; i++)
			{
				code[i]=numbers[i];
				set_menu(MENU_GENERAL_SETUP);
			}
		}
		else
		{
			uint8_t error=0;
			for(uint8_t i=0; i < 4; i++)
			{
				if(code[i]!=numbers[i])
					error=1;

			}

			if(error==0)
			{
				set_menu(MENU_MAIN);
			}
			else
			{
				lcd_gotoxy(8,2);
				lcd_puts("    ");
			}
		}
	}
}

void setup_reset(void)
{
	current_menu=0;
	setup_menu_show();
}

