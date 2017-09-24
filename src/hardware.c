#include "hardware.h"
#include <avr/io.h>

#define BUTTON_ROW_1 0 //change set_row if changed!!!
#define BUTTON_ROW_2 1
#define BUTTON_ROW_3 2
#define BUTTON_ROW_4 3


#define BUTTON_ROW_PORT PORTA
#define BUTTON_ROW_DDR DDRA
#define BUTTON_ROW_PIN PINA
#define BUTTON_COL_PORT PORTB
#define BUTTON_COL_DDR DDRB
#define BUTTON_COL_PIN PINB

#define CAMKEY_POS 0
#define CAMKEY_1 (button_states[0]&(1<CAMKEY_POS))
#define CAMKEY_2 (button_states[1]&(1<CAMKEY_POS))
#define CAMKEY_3 (button_states[2]&(1<CAMKEY_POS))
#define CAMKEY_4 (button_states[3]&(1<CAMKEY_POS))

#define STOREKEY_POS 1
#define STOREKEY_1 (button_states[0]&(1<STOREKEY_POS))
#define STOREKEY_2 (button_states[1]&(1<STOREKEY_POS))
#define STOREKEY_3 (button_states[2]&(1<STOREKEY_POS))
#define STOREKEY_4 (button_states[3]&(1<STOREKEY_POS))

#define SOFTKEY_POS 2
#define SOFTKEY_1 (button_states[0]&(1<SOFTKEY_POS))
#define SOFTKEY_2 (button_states[1]&(1<SOFTKEY_POS))
#define SOFTKEY_3 (button_states[2]&(1<SOFTKEY_POS))
#define SOFTKEY_4 (button_states[3]&(1<SOFTKEY_POS))


static uint8_t button_states[4];

void hardware_init(void)
{
	BUTTON_COL_DDR = 0x00; //All input
	BUTTON_ROW_DDR &= ~((1<<BUTTON_ROW_1)|(1<<BUTTON_ROW_2)|(1<<BUTTON_ROW_3)|(1<<BUTTON_ROW_4));


	hardware_tick();
	hardware_tick();
	hardware_tick();
	hardware_tick();
}

static void set_row(uint8_t row)
{
	switch(row)
	{
		case 0: BUTTON_ROW_DDR =  (BUTTON_ROW_DDR & 0xF0)  | 0b0001; break;
		case 1: BUTTON_ROW_DDR =  (BUTTON_ROW_DDR & 0xF0)  | 0b0010; break;
		case 2: BUTTON_ROW_DDR =  (BUTTON_ROW_DDR & 0xF0)  | 0b0100; break;
		case 3: BUTTON_ROW_DDR =  (BUTTON_ROW_DDR & 0xF0)  | 0b1000; break;
	}
}

void hardware_tick(void)
{
	static uint8_t row=0;
	set_row(row);

	asm("NOP");
	asm("NOP");
	asm("NOP");

	button_states[row]= BUTTON_COL_PIN;

	row++;
	if(row>3)
		row=0;
}

camkey_t get_camkeys(void)
{	
	static uint8_t old_k1=0;
//static uint8_t old_k2=0;
//	static uint8_t old_k3=0;
//	static uint8_t old_k4=0;


	if(CAMKEY_1 )
	{
		if(old_k1==0)
		{
			old_k1=1;
			return CAM1;
		}
	}
	else
	{
		old_k1=0;
	}

	if(CAMKEY_2)
		return CAM2;
	if(CAMKEY_3)
		return CAM3;
	if(CAMKEY_4)
		return CAM4;
	return CAM_NO_KEY;
}

uint8_t get_cam1_key(void)
{
	//return (CAMKEY_PIN & (1<<CAMKEY_1));
}

softkey_t get_softkeys(void)
{	
	static uint8_t old_k1=0;
	static uint8_t old_k2=0;
	static uint8_t old_k3=0;
	static uint8_t old_k4=0;


	if( SOFTKEY_1)
	{
		if(old_k1==0)
		{
			old_k1=1;
			return SK1;
		}
	}
	else
	{
		if(old_k1==1)
		{
			old_k1=0;	
			return SK1_R;
		}
	}

	if(SOFTKEY_2)
	{
		if(old_k2==0)
		{
			old_k2=1;
			return SK2;
		}
	}
	else
	{
		if(old_k2==1)
		{
			old_k2=0;
			return SK2_R;
		}
	}


	if(SOFTKEY_3)
	{
		if(old_k3==0)
		{
			old_k3=1;
			return SK3;
		}
	}
	else
	{
		if(old_k3==1)
		{
			old_k3=0;
			return SK3_R;
		}
	}


	if(SOFTKEY_4)
	{
		if(old_k4==0)
		{
			old_k4=1;
			return SK4;
		}
	}
	else
	{
		if(old_k4==1)
		{
			old_k4=0;
			return SK4_R;
		}
	}

	return NO_KEY;
}

storekey_t get_storekeys(void)
{	

	if( STOREKEY_1)
		return STORE1;
	if( STOREKEY_2)
		return STORE2;
	if( STOREKEY_3)
		return STORE3;
	if( STOREKEY_4)
		return STORE4;
	return STORE_NO_KEY;
}

rotselkey_t get_rotselkeys(void)
{
	return ROTSEL1;
}

void set_store_led(uint8_t led)
{
	
}

void reset_store_led(uint8_t led)
{

}

void set_cam_leds(uint8_t active)
{

}

void set_rotarys_leds(uint8_t number)
{


}


