#include "menu_structures.h"

void main_show(void);
void setup_show_cam(void);
void setup_cam_down(void);
void setup_cam_up(void);
void setup_menu_prev(void);
void setup_menu_next(void);
void setup_menu_enter(void);
void save_data(void);
void param_resetId(void);
void param_show(void);
void param_next(void);
void param_up(void);
void param_up(void);
void store_clear(void);
void setup_reset(void);
void load_default(void);
void ctrl_cam_show(void);
void cam_power_show(void);
void ctrl_cam_down(void);
void ctrl_cam_up(void);
void cam_power_on(void);
void cam_power_off(void);
void cam_button(void);
void lock_1_pressed(void);
void lock_2_pressed(void);
void lock_3_pressed(void);
void lock_4_pressed(void);

static const char MENU_SPLASH_L1[] PROGMEM			= "DragonVideo         ";
static const char MENU_SPLASH_L2[] PROGMEM			= "By Karrn            ";
static const char MENU_SPLASH_L3[] PROGMEM			= "Cam controller      ";
static const char MENU_SPLASH_L4[] PROGMEM			= "                2017";

static const char MENU_MAIN_L1[] PROGMEM			= "                    ";
static const char MENU_MAIN_L2[] PROGMEM			= "                    ";
static const char MENU_MAIN_L3[] PROGMEM			= "                    ";
static const char MENU_MAIN_L4[] PROGMEM			= "STORE          SETUP";

static const char MENU_SETUP_L1[] PROGMEM			= "Setup               ";
static const char MENU_SETUP_L2[] PROGMEM			= "                    ";
static const char MENU_SETUP_L3[] PROGMEM			= "                    ";
static const char MENU_SETUP_L4[] PROGMEM			= "PREV NEXT EDIT  BACK";

static const char MENU_EDIT_CAM_L1[] PROGMEM		= "Setup               ";
static const char MENU_EDIT_CAM_L2[] PROGMEM		= "                    ";
static const char MENU_EDIT_CAM_L3[] PROGMEM		= "                    ";
static const char MENU_EDIT_CAM_L4[] PROGMEM		= "NEXT UP   DOWN  BACK";

static const char MENU_STORE_L1[] PROGMEM			= "Store               ";
static const char MENU_STORE_L2[] PROGMEM			= "Choose store to save";
static const char MENU_STORE_L3[] PROGMEM			= "                    ";
static const char MENU_STORE_L4[] PROGMEM			= "     CLEAR     ABORT";

static const char MENU_CLEAR_L1[] PROGMEM			= "Clear store         ";
static const char MENU_CLEAR_L2[] PROGMEM			= "Choose store        "; 
static const char MENU_CLEAR_L3[] PROGMEM			= "                    ";
static const char MENU_CLEAR_L4[] PROGMEM			= "ALL            ABORT";

static const char MENU_GENERAL_SETUP_L1[] PROGMEM	= "Choose setup menu   ";
static const char MENU_GENERAL_SETUP_L2[] PROGMEM	= "                    ";
static const char MENU_GENERAL_SETUP_L3[] PROGMEM	= "                    ";
static const char MENU_GENERAL_SETUP_L4[] PROGMEM	= "PREV NEXT ENTER BACK";

static const char MENU_RESET_L1[] PROGMEM			= "Setup               ";
static const char MENU_RESET_L2[] PROGMEM			= "Load default config ";
static const char MENU_RESET_L3[] PROGMEM			= "and erase all data? ";
static const char MENU_RESET_L4[] PROGMEM			= "YES             BACK";

static const char MENU_CTRL_L1[] PROGMEM			= "Setup               ";
static const char MENU_CTRL_L2[] PROGMEM			= "                    ";
static const char MENU_CTRL_L3[] PROGMEM			= "                    ";
static const char MENU_CTRL_L4[] PROGMEM			= "PREV NEXT CTRL  BACK";

static const char MENU_CTRL_EDIT_L1[] PROGMEM		= "Setup               ";
static const char MENU_CTRL_EDIT_L2[] PROGMEM		= "                    ";
static const char MENU_CTRL_EDIT_L3[] PROGMEM		= "  Power             ";
static const char MENU_CTRL_EDIT_L4[] PROGMEM		= " ON  OFF   BTN  BACK";

static const char MENU_LOCKED_L1[] PROGMEM			= "    DragonVideo     ";
static const char MENU_LOCKED_L2[] PROGMEM			= "  **** LOCKED ****  ";
static const char MENU_LOCKED_L3[] PROGMEM			= "                    ";
static const char MENU_LOCKED_L4[] PROGMEM			= " 1    2     3     4 ";

static const char MENU_LOCK_SETUP_L1[] PROGMEM		= "Setup               ";
static const char MENU_LOCK_SETUP_L2[] PROGMEM		= "Enter new code:     ";
static const char MENU_LOCK_SETUP_L3[] PROGMEM		= "                    ";
static const char MENU_LOCK_SETUP_L4[] PROGMEM		= " 1    2     3     4 ";

//Define the menu structure
__flash const menu_t menues[] =
{ 	
	{ //MENU_SPLASH
		.lines = { MENU_SPLASH_L1,MENU_SPLASH_L2,MENU_SPLASH_L3,MENU_SPLASH_L4},
		.next =  { MENU_MAIN,MENU_MAIN,MENU_MAIN,MENU_MAIN},
		.cb =    { NULL,NULL,NULL,NULL},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init =  NULL
    },
	{ //MENU_MAIN
		.lines = { MENU_MAIN_L1,MENU_MAIN_L2,MENU_MAIN_L3,MENU_MAIN_L4},
		.next  = { MENU_STORE, MENU_INVALID,MENU_INVALID,MENU_GENERAL_SETUP},
		.cb    = { NULL,NULL,NULL,NULL},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = main_show
	},
	{ //MENU_SETUP
		.lines = { MENU_SETUP_L1,MENU_SETUP_L2,MENU_SETUP_L3,MENU_SETUP_L4},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_EDIT_CAM,MENU_GENERAL_SETUP},
		.cb    = { setup_cam_down,setup_cam_up,param_resetId,NULL},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init =  setup_show_cam
	},
	{ //MENU_EDIT_CAM
		.lines = { MENU_EDIT_CAM_L1,MENU_EDIT_CAM_L2,MENU_EDIT_CAM_L3,MENU_EDIT_CAM_L4},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_SETUP},
		.cb    = { param_next,param_up, param_up, NULL},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = param_show
	},
	{ //MENU_STORE
		.lines = { MENU_STORE_L1,MENU_STORE_L2,MENU_STORE_L3,MENU_STORE_L4},
		.next  = { MENU_INVALID,MENU_CLEAR,MENU_INVALID,MENU_MAIN},
		.cb    = { NULL,NULL, NULL, NULL},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = NULL
	},
	{ //MENU_CLEAR
		.lines = { MENU_CLEAR_L1,MENU_CLEAR_L2,MENU_CLEAR_L3,MENU_CLEAR_L4},
		.next  = { MENU_MAIN,MENU_INVALID,MENU_INVALID,MENU_MAIN},
		.cb    = { store_clear,NULL, NULL, NULL},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = NULL
	}, 
	{ //MENU_GENERAL_SETUP
		.lines = { MENU_GENERAL_SETUP_L1,MENU_GENERAL_SETUP_L2,MENU_GENERAL_SETUP_L3,MENU_GENERAL_SETUP_L4},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_MAIN},
		.cb    = { setup_menu_prev,setup_menu_next, setup_menu_enter, save_data},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = setup_reset
	}, 
	{ //MENU_RESET
		.lines = { MENU_RESET_L1,MENU_RESET_L2,MENU_RESET_L3,MENU_RESET_L4},
		.next  = { MENU_MAIN,MENU_INVALID,MENU_INVALID,MENU_MAIN},
		.cb    = { load_default,NULL, NULL, NULL},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = NULL
	},  
	{ //MENU_CTRL
		.lines = { MENU_CTRL_L1,MENU_CTRL_L2,MENU_CTRL_L3,MENU_CTRL_L4},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_CTRL_EDIT,MENU_GENERAL_SETUP},
		.cb    = { ctrl_cam_down,ctrl_cam_up, NULL, NULL},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = ctrl_cam_show
	},  
	{ //MENU_CTRL_EDIT
		.lines = { MENU_CTRL_EDIT_L1,MENU_CTRL_EDIT_L2,MENU_CTRL_EDIT_L3,MENU_CTRL_EDIT_L4},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_CTRL_EDIT,MENU_CTRL},
		.cb    = { cam_power_on,cam_power_off, cam_button, NULL},
		.cb_r=   { NULL,NULL,cam_button,NULL},
		.init  = cam_power_show
	} ,  
	{ //MENU_LOCKED
		.lines = { MENU_LOCKED_L1,MENU_LOCKED_L2,MENU_LOCKED_L3,MENU_LOCKED_L4},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_INVALID},
		.cb    = { lock_1_pressed,lock_2_pressed, lock_3_pressed, lock_4_pressed},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = NULL
	} ,  
	{ //MENU_LOCK_SETUP
		.lines = { MENU_LOCK_SETUP_L1,MENU_LOCK_SETUP_L2,MENU_LOCK_SETUP_L3,MENU_LOCK_SETUP_L4},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_INVALID},
		.cb    = {lock_1_pressed,lock_2_pressed, lock_3_pressed, lock_4_pressed},
		.cb_r=   { NULL,NULL,NULL,NULL},
		.init  = NULL
	}
};


