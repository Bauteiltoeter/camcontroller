#include "menu_all_power.h"
#include "globals.h"

extern void send_switch_state(uint8_t cam);

void switch_cams_on(void)
{
	for(uint8_t i=0; i < CAM_COUNT; i++)
	{
		cams[i].power_state=1;
		send_switch_state(i);
	}
}

void switch_cams_off(void)
{
	for(uint8_t i=0; i < CAM_COUNT; i++)
	{
		cams[i].power_state=0;
		send_switch_state(i);
	}
}
