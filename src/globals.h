#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdint.h>

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

extern cam_data_t cams[CAM_COUNT];	//Data for each cam
extern uint8_t active_cam; //remember the active cam
#endif
