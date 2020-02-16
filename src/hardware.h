/**
 * \file hardware.h
 * \author Torben Hellige
 * \brief This file provides an abstraction layer to the hardware. All hardware-related calls for LEDs and buttons are done here
 */

#ifndef __HARDWARE_H
#define __HARDWARE_H

#include <stdint.h>

/**
 * \enum softkey_t
 *
 *
 */ 
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
		FIX_NO_KEY = 255,
		FIX1 = 0,
		FIX2 = 1,
		FIX3 = 2,
		FIX4 = 3
	} fixturekey_t;

	typedef enum {
        FSEL_NO_KEY = 255,
        FSEL1 = 0,
        FSEL2 = 1,
        FSEL3 = 2,
        FSEL4 = 3,
		FSEL5 = 4,
		FSEL6 = 5,
		FSEL7 = 6,
		FSEL8 = 7
    } fselkey_t;

	void hardware_tick(void);
	void hardware_init(void);

	softkey_t get_softkeys(void);
	fixturekey_t get_fixturekeys(void);
    fselkey_t get_fselkeys(void);

	void set_fixture_leds(uint8_t active);
    void set_fsel_leds(uint8_t function,uint8_t bank);
	void set_bank_led(uint8_t on);
	void set_rotarys_leds(uint8_t number);
	uint8_t get_matrix_line(uint8_t line);

#endif
