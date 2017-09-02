#ifndef __MENU_STRUCTURE_H
#define __MENU_STRUCTURE_H

	#include <stdint.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

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

	extern menu_t menues[];

#endif
