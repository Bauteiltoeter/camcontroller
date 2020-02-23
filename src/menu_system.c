#include "menu_system.h"
#include "menu_main.h"
#include "menu_lock.h"
#include "menu_settings_general.h"
#include "menu_hardware.h"
#include "menu_dmxaddr.h"
#include "menu_scenes.h"
#include "menu_playback.h"
#include "menu_times.h"
#include "lcd.h"
#include "hardware.h"


extern void save_data(void);
extern void load_default(void);
extern void lock_system(void); 

static const char MENU_SPLASH_L1[] PROGMEM			= "DragonWolfTech      ";
static const char MENU_SPLASH_L2[] PROGMEM			= "SimpleDMX controller";
static const char MENU_SPLASH_L3[] PROGMEM			= "Karrn und Sternchen ";
static const char MENU_SPLASH_L4[] PROGMEM			= "                2020";

static const char MENU_MAIN_L1[] PROGMEM			= "                    ";
static const char MENU_MAIN_L2[] PROGMEM			= "                    ";
static const char MENU_MAIN_L3[] PROGMEM			= "                    ";
static const char MENU_MAIN_L4[] PROGMEM			= "SCENE TIME PLAY MENU";

static const char MENU_GENERAL_SETUP_L1[] PROGMEM	= "Choose menu option  ";
static const char MENU_GENERAL_SETUP_L2[] PROGMEM	= "                    ";
static const char MENU_GENERAL_SETUP_L3[] PROGMEM	= "                    ";
static const char MENU_GENERAL_SETUP_L4[] PROGMEM	= "PREV NEXT ENTER BACK";

static const char MENU_RESET_L1[] PROGMEM			= "Setup               ";
static const char MENU_RESET_L2[] PROGMEM			= "Load default config ";
static const char MENU_RESET_L3[] PROGMEM			= "and erase all data? ";
static const char MENU_RESET_L4[] PROGMEM			= "YES             BACK";

static const char MENU_LOCKED_L1[] PROGMEM			= "    DragonVideo     ";
static const char MENU_LOCKED_L2[] PROGMEM			= "  **** LOCKED ****  ";
static const char MENU_LOCKED_L3[] PROGMEM			= "                    ";
static const char MENU_LOCKED_L4[] PROGMEM			= " 1    2     3     4 ";

static const char MENU_LOCK_SETUP_L1[] PROGMEM		= "Setup               ";
static const char MENU_LOCK_SETUP_L2[] PROGMEM		= "Enter new code:     ";
static const char MENU_LOCK_SETUP_L3[] PROGMEM		= "                    ";
static const char MENU_LOCK_SETUP_L4[] PROGMEM		= " 1    2     3     4 ";

static const char MENU_HARDWARE_L1[] PROGMEM 		= "                    ";
static const char MENU_HARDWARE_L2[] PROGMEM 		= "                    ";
static const char MENU_HARDWARE_L3[] PROGMEM 		= "                    ";
static const char MENU_HARDWARE_L4[] PROGMEM 		= "                BACK";

static const char MENU_LED_BRIGHTNESS_L1[] PROGMEM  = "Setup               ";
static const char MENU_LED_BRIGHTNESS_L2[] PROGMEM  = "                    ";
static const char MENU_LED_BRIGHTNESS_L3[] PROGMEM  = "                    ";
static const char MENU_LED_BRIGHTNESS_L4[] PROGMEM  = "UP  DOWN        BACK";

static const char MENU_LED_DMXADDR_L1[] PROGMEM     = "DMX Address setup   ";
static const char MENU_LED_DMXADDR_L2[] PROGMEM     = "                    ";
static const char MENU_LED_DMXADDR_L3[] PROGMEM     = "                    ";
static const char MENU_LED_DMXADDR_L4[] PROGMEM     = "AUTO      NEXT  BACK";

static const char MENU_SCENES_L1[] PROGMEM     		= "Scene menu          ";
static const char MENU_SCENES_L2[] PROGMEM     		= "                    ";
static const char MENU_SCENES_L3[] PROGMEM     		= "                    ";
static const char MENU_SCENES_L4[] PROGMEM     		= "LOAD  DEL  SAVE BACK";

static const char MENU_TIMES_L1[] PROGMEM     		= "Configure times:    ";
static const char MENU_TIMES_L2[] PROGMEM     		= "                    ";
static const char MENU_TIMES_L3[] PROGMEM     		= "                    ";
static const char MENU_TIMES_L4[] PROGMEM     		= "CROSS TIME PLAY BACK";

static const char MENU_PLAYBACK_L1[] PROGMEM     	= "Playback            ";
static const char MENU_PLAYBACK_L2[] PROGMEM     	= "Current scene:      ";
static const char MENU_PLAYBACK_L3[] PROGMEM     	= "                    ";
static const char MENU_PLAYBACK_L4[] PROGMEM     	= "                STOP";




//Define the menu structure
__flash const menu_t menues[] =
{ 	
	{ //MENU_SPLASH
		.lines = { MENU_SPLASH_L1,MENU_SPLASH_L2,MENU_SPLASH_L3,MENU_SPLASH_L4},
		.next =  { MENU_MAIN,MENU_MAIN,MENU_MAIN,MENU_MAIN, MENU_MAIN},
		.cb =    { NULL,NULL,NULL,NULL,NULL},
		.cb_r=   { NULL,NULL,NULL,NULL,NULL},
		.init =  NULL,
		.cyclic = NULL,
		.rotary= NULL
    },
	{ //MENU_MAIN
		.lines = { MENU_MAIN_L1,MENU_MAIN_L2,MENU_MAIN_L3,MENU_MAIN_L4},
		.next  = { MENU_SCENES, MENU_TIMES,MENU_PLAY,MENU_GENERAL_SETUP,MENU_INVALID},
		.cb    = { NULL,NULL,NULL,NULL,NULL},
		.cb_r=   { NULL,NULL,NULL,NULL,NULL},
		.init  = main_init,
        .cyclic = main_run,
		.rotary= NULL //the main menu uses the rotary encoder, but it's initialised in the main_init
	},
	{ //MENU_GENERAL_SETUP
		.lines = { MENU_GENERAL_SETUP_L1,MENU_GENERAL_SETUP_L2,MENU_GENERAL_SETUP_L3,MENU_GENERAL_SETUP_L4},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_MAIN,MENU_INVALID},
		.cb    = { setup_menu_prev,setup_menu_next, setup_menu_enter, save_data,setup_menu_enter},
		.cb_r=   { NULL,NULL,NULL,NULL,NULL},
		.init  = setup_reset,
		.cyclic = NULL,
		.rotary= &rotary_menu_settings_general
	}, 
	{ //MENU_RESET
		.lines = { MENU_RESET_L1,MENU_RESET_L2,MENU_RESET_L3,MENU_RESET_L4},
		.next  = { MENU_MAIN,MENU_INVALID,MENU_INVALID,MENU_MAIN,MENU_INVALID},
		.cb    = { load_default,NULL, NULL, NULL,NULL},
		.cb_r=   { NULL,NULL,NULL,NULL,NULL},
		.init  = NULL,
		.cyclic = NULL,
		.rotary= NULL
	},   
	{ //MENU_LOCKED
		.lines = { MENU_LOCKED_L1,MENU_LOCKED_L2,MENU_LOCKED_L3,MENU_LOCKED_L4},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_INVALID},
		.cb    = { lock_1_pressed,lock_2_pressed, lock_3_pressed, lock_4_pressed,NULL},
		.cb_r=   { NULL,NULL,NULL,NULL,NULL},
		.init  = lock_system,
		.cyclic = NULL,
		.rotary= NULL
	} ,  
	{ //MENU_LOCK_SETUP
		.lines = { MENU_LOCK_SETUP_L1,MENU_LOCK_SETUP_L2,MENU_LOCK_SETUP_L3,MENU_LOCK_SETUP_L4},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_INVALID},
		.cb    = {lock_1_pressed,lock_2_pressed, lock_3_pressed, lock_4_pressed,NULL},
		.cb_r=   { NULL,NULL,NULL,NULL,NULL},
		.init  = NULL,
		.cyclic = NULL,
		.rotary= NULL
	},
	{ //MENU_HARDWARE
		.lines = { MENU_HARDWARE_L1,MENU_HARDWARE_L2,MENU_HARDWARE_L3,MENU_HARDWARE_L4},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_GENERAL_SETUP,MENU_INVALID},
		.cb    = { NULL,NULL,NULL,NULL,NULL},
		.cb_r=   { NULL,NULL,NULL,NULL,NULL},
		.init  = NULL,
		.cyclic = show_hardware_info,
		.rotary= NULL
	} ,
	{ //MENU_LED_BRIGHTNESS
		.lines = { MENU_LED_BRIGHTNESS_L1,MENU_LED_BRIGHTNESS_L2,MENU_LED_BRIGHTNESS_L3,MENU_LED_BRIGHTNESS_L4},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_GENERAL_SETUP,MENU_INVALID},
		.cb    = { NULL,NULL,NULL,NULL,NULL},
		.cb_r=   { NULL,NULL,NULL,NULL,NULL},
		.init  = NULL,
		.cyclic = NULL,
		.rotary= NULL
	}, //MENU_DMXADDR 
	{
		.lines = { MENU_LED_DMXADDR_L1,MENU_LED_DMXADDR_L2,MENU_LED_DMXADDR_L3,MENU_LED_DMXADDR_L4},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_MAIN,MENU_INVALID},
		.cb    = { dmxaddr_auto,NULL,dmxaddr_next,dmxaddr_save,NULL},
		.cb_r=   { NULL,NULL,NULL,NULL,NULL},
		.init  = dmxaddr_init,
		.cyclic = NULL,
		.rotary= NULL
	}, //MENU_SCENES
	{
		.lines = { MENU_SCENES_L1,MENU_SCENES_L2,MENU_SCENES_L3,MENU_SCENES_L4},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_MAIN,MENU_INVALID},
		.cb    = { scenes_load,scenes_delete,scenes_save,NULL,NULL},
		.cb_r=   { NULL,NULL,NULL,NULL,NULL},
		.init  = scenes_init,
		.cyclic = NULL,
		.rotary= &rotary_scenes_menu
	}, //MENU_TIMES
	{
		.lines = { MENU_TIMES_L1,MENU_TIMES_L2,MENU_TIMES_L3,MENU_TIMES_L4},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_MAIN,MENU_INVALID},
		.cb    = { times_cross,times_time,times_play,times_save,NULL},
		.cb_r=   { NULL,NULL,NULL,NULL,NULL},
		.init  = times_init,
		.cyclic = times_update,
		.rotary= NULL
	}, //MENU_PLAYBACK 
	{
		.lines = { MENU_PLAYBACK_L1,MENU_PLAYBACK_L2,MENU_PLAYBACK_L3,MENU_PLAYBACK_L4},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_MAIN,MENU_INVALID},
		.cb    = { NULL,NULL,NULL,NULL,NULL},
		.cb_r=   { NULL,NULL,NULL,NULL,NULL},
		.init  = playback_init,
		.cyclic = playback_update,
		.rotary= NULL
	}
};

menu_identifiers active_menu;

void process_menu(void)
{
	softkey_t button = get_softkeys();

	if(button != NO_KEY )
	{
		menu_identifiers old;
		switch(button)
		{
			case SK1: 
			case SK2: 
			case SK3:
			case SK4: 
			case ROT:
				old = active_menu;

				if(menues[active_menu].cb[button] != NULL)
						menues[active_menu].cb[button]();	

				if(active_menu == old && menues[active_menu].next[button] != MENU_INVALID)
					set_menu(menues[active_menu].next[button]);
			break;
			default:
				if(menues[active_menu].cb_r[button-5] != NULL)
						menues[active_menu].cb_r[button-5]();	

			break;
					
		}
	}	

	if(menues[active_menu].cyclic)
		menues[active_menu].cyclic();
}

void set_menu(menu_identifiers menu)
{
	active_menu = menu;
	lcd_clrscr();
	lcd_puts_p(menues[menu].lines[0]);
	lcd_gotoxy(0,1);
	lcd_puts_p(menues[menu].lines[1]);
	lcd_gotoxy(0,2);
	lcd_puts_p(menues[menu].lines[2]);
	lcd_gotoxy(0,3);
	lcd_puts_p(menues[menu].lines[3]);

	//NULL is also valid here, so no NULL check (disable rotary)
	rotary_setconfig(menues[menu].rotary);

	if(menues[menu].init)
		menues[menu].init();
}





