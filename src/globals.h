/**
 * \file globals.h
 * \author Torben Hellige
 * \brief This file provides the declarations for some global variables containing importen informations about the system state
 *
 *
 */


#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdint.h>

#define MOVING_FIXTURE_COUNT 2 	//!< Number of cameras in the system
#define STATIC_FIXTURE_COUNT 4 	//!< Number of stores for each cam

/**
 * \struct cam_data_t 
 * This struct contains all relevant data for one fixture. It stores the DMX addresses, dmx values, setup parameter and positions
 */
typedef struct  {
	uint16_t pan;
	uint16_t tilt;
	uint16_t base_addr;
	uint8_t focus;
} moving_fixture_data_t;


extern moving_fixture_data_t cams[MOVING_FIXTURE_COUNT];	//!< global array to store the configuration data for each fixture
extern uint8_t active_fixture; 		//!< global variable that contains the number of the currently manipulated fixture
extern uint8_t active_rotary_funtion;
#endif
