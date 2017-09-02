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
		.cb    = { param_next,param_up, param_up, NULL},
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
