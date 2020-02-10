#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdlib.h>

#include <string.h>
#include "lcd.h"
#include "dmx.h"
#include "adc.h"
#include "hardware.h"
#include "rotary.h"
#include "menu_system.h"
#include "menu_main.h"
#include "menu_lock.h"
#include "globals.h"




void save_data(void);

void process_inputs(void);
void load_default(void);

void lock_system(void);
void unlock_system(void);




//###########################
//  Global variables
//###########################



//const moving_fixture_data_t moving_fixture_default[MOVING_FIXTURE_COUNT] PROGMEM = 
//{
//
//};

//cam_data_t backup_cams[CAM_COUNT] EEMEM; //store CAM Data in eeprom
uint8_t menu_lock_active EEMEM;
//Hauptprogramm
int main (void) 
{  
	//initialise stuff
	hardware_init();
//	eeprom_read_block (cams, backup_cams, sizeof(cams));
	lock_load();

	lcd_init(LCD_DISP_ON);

	uint8_t lock_active=eeprom_read_byte(&menu_lock_active);
	
	//if the system was locked during last shutdown and camkey 1 is not pressed lock the system
	if(lock_active==1 /*&& !get_cam1_key()*/)
	{
		set_menu(MENU_LOCKED);
	}
	else
	{
		if(lock_active==255 /*|| get_cam1_key()*/)
			unlock_system();
		set_menu(MENU_SPLASH);
	}

	dmx_init();
	ADC_Init();
	rotary_init();
//
//	send_switch();

//	set_cam_leds(active_cam);

//	for(uint8_t i=0; i < CAM_COUNT; i++)
//	{
//		if(cams[i].cam_active)
//		{
//			write_dmx(cams[i].base_addr-1 + cams[i].pan_address, cams[i].pan);
//			write_dmx(cams[i].base_addr-1 + cams[i].tilt_address, cams[i].tilt);
//			write_dmx(cams[i].base_addr-1 + cams[i].speed_address, cams[i].speed);
//			send_switch_state(i);
//		}
//	}


	uint16_t blink_counter=0;
	uint8_t loop=0;

	while(1)
	{
		blink_counter++;
		loop++;

		if(loop==10)
		{  
			blink_counter++;
			if(active_menu != MENU_LOCKED)
			{
				//Process analog inputs, cam changes and store requests
				process_inputs();
				rotary_process();
			}

			//Process the menu system  / softkeys
			process_menu();
		
	
			loop=0;
		}
		


		hardware_tick();
		rotary_tick();

		//awkward delay here.. should be replaced by a timer. But it works, DMX is interrupt-driven
		_delay_ms(0.5);
	}
}


void process_inputs(void)
{
	
}

void save_data(void)
{
	//eeprom_write_block (cams, backup_cams, sizeof(cams));
	lock_save();
}

void load_default(void)
{

	//memcpy_P(cams,cams_default, sizeof(cams)); 
	save_data();
}

void lock_system(void)
{
	eeprom_write_byte(&menu_lock_active, 1);
}

void unlock_system(void)
{
	eeprom_write_byte(&menu_lock_active, 0);
}

