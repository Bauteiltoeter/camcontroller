#ifndef __HARDWARE_H
#define __HARDWARE_H

#include <stdint.h>

	typedef enum {
		NO_KEY=255,
		SK1=0,
		SK2=1,
		SK3=2,
		SK4=3,
		ROT=4,
		SK1_R = 5,
		SK2_R = 6,
		SK3_R = 7,
		SK4_R = 8,
		ROT_R = 9
	} softkey_t;

	typedef enum {
		CAM_NO_KEY = 255,
		CAM1 = 0,
		CAM2 = 1,
		CAM3 = 2,
		CAM4 = 3
	} camkey_t;

	typedef enum {
		STORE_NO_KEY = 255,
		STORE1 = 0,
		STORE2 = 1,
		STORE3 = 2,
		STORE4 = 3
	} storekey_t;

	typedef enum {
		ROTSEL_NO_KEY = 255,
		ROTSEL1 = 0,
		ROTSEL2 = 1,
		ROTSEL3 = 2,
		ROTSEL4 = 3
	} rotselkey_t;

	void hardware_init(void);
	softkey_t get_softkeys(void);
	camkey_t get_camkeys(void);
	storekey_t get_storekey(void);
	rotselkey_t get_rotselkey(void);
	void set_store_led(uint8_t led);
	void reset_store_led(uint8_t led);
	void set_cam_leds(uint8_t active);

#endif
