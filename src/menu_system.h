#ifndef __MENU_STRUCTURE_H
#define __MENU_STRUCTURE_H

	#include <stdint.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "rotary.h"

	typedef enum {
		MENU_SPLASH 		=0,
		MENU_MAIN  			= 1,
		MENU_STORE 			= 2,
		MENU_CLEAR 			= 3,
		MENU_GENERAL_SETUP 	= 4,
		MENU_RESET  		= 5,
		MENU_LOCKED 		= 6,
		MENU_LOCK_SETUP 	= 7,
		MENU_HARDWARE   	= 8,
		MENU_LED_BRIGHTNESS = 9,
		MENU_DMXADDR		= 10,
		MENU_INVALID 		= 255
	} menu_identifiers;


	typedef void(*menu_button)(void);
	typedef void(*init_function)(void);
	typedef void(*cyclic_function)(void);

	// This struct defines a menu page
	// Each page contains the 4 LCD lines, the ID of the reachable menues, callbacks for each buttons and an init-function called when opening the menu
	//
	typedef struct
	{
		const char* lines[4];				//Display content
		menu_identifiers next[5];	//Menu to reach if a softkey is pressed
		menu_button cb[5];			//Callback for buttonpresses
		menu_button cb_r[5];
		init_function init;			//Init function
		cyclic_function cyclic;
		rotary_config_t* rotary;
	} menu_t;

	extern __flash const menu_t menues[];
	extern menu_identifiers active_menu;

	void set_menu(menu_identifiers menu);
	void process_menu(void);
#endif
