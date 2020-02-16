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
#define CAMEOTRI_FIXTURE_COUNT 6 	//!< Number of stores for each cam
#define CAMEOFLOOD_FIXTURE_COUNT 3
#define CITYCOLOR_FIXTURE_COUNT 3

#define TOTAL_FIXTURE_COUNT (MOVING_FIXTURE_COUNT + CAMEOTRI_FIXTURE_COUNT + CAMEOFLOOD_FIXTURE_COUNT + CITYCOLOR_FIXTURE_COUNT)
/**
 * \struct cam_data_t 
 * This struct contains all relevant data for one fixture. It stores the DMX addresses, dmx values, setup parameter and positions
 */
typedef struct  {
	uint16_t dmx_addr;
	uint8_t shutter; 
	uint16_t pan;
	uint16_t tilt;
	uint8_t focus;
	uint8_t rotf;
	uint8_t rot;
	uint8_t speed;
	uint8_t selected;
} imagescan_data_t;

typedef struct {
	uint16_t dmx_addr;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t dimmer;
	uint8_t strobe;
	uint8_t color;
	uint8_t selected;
} cameotri_data_t;

typedef struct {
	uint16_t dmx_addr;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t white;
	uint8_t amber;
	uint8_t uv;
	uint8_t dimmer;
	uint8_t strobe;
	uint8_t duration;
	uint8_t selected;
} cameoflood_data_t;

typedef struct {
	uint16_t dmx_addr;
	uint8_t cyan;
	uint8_t magenta;
	uint8_t yellow;
	uint8_t motorspeed;
	uint8_t basiccolors;
	uint8_t lamp;
	uint8_t selected;
} citycolor_data_t;



typedef enum {
	type_uint16,
	type_uint8
} parameter_type;


extern imagescan_data_t imageScan_data[MOVING_FIXTURE_COUNT];	//!< global array to store the configuration data for each fixture
extern cameotri_data_t cameotri_data[CAMEOTRI_FIXTURE_COUNT];	//!< global array to store the configuration data for each fixture
extern cameoflood_data_t cameoflood_data[CAMEOFLOOD_FIXTURE_COUNT];	//!< global array to store the configuration data for each fixture
extern citycolor_data_t citycolor_data[CITYCOLOR_FIXTURE_COUNT];	//!< global array to store the configuration data for each fixture
extern uint8_t active_fixture; 		//!< global variable that contains the number of the currently manipulated fixture
extern uint8_t active_rotary_funtion;
#endif
