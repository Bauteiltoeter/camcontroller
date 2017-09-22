#include "hardware.h"
#include <avr/io.h>

	#define SOFTKEY_DDR DDRC
	#define SOFTKEY_PORT PORTC
	#define SOFTKEY_PIN PINC

	#define SOFTKEY_1 0
	#define SOFTKEY_2 1
	#define SOFTKEY_3 2
	#define SOFTKEY_4 3

	#define CAMKEY_DDR DDRC
	#define CAMKEY_PORT PORTC
	#define CAMKEY_PIN PINC
	#define CAMKEY_1 4
	#define CAMKEY_2 5
	#define CAMKEY_3 6
	#define CAMKEY_4 7

	#define STOREKEY_DDR DDRD
	#define STOREKEY_PORT PORTD
	#define STOREKEY_PIN PIND

	#define STORE_1 7
	#define STORE_2 6
	#define STORE_3 5
	#define STORE_4 4

	#define LED_CAM_1_DDR DDRA
	#define LED_CAM_1_PORT PORTA
	#define LED_CAM_1 7

	#define LED_CAM_2_DDR DDRB
	#define LED_CAM_2_PORT PORTB
	#define LED_CAM_2 7

	#define LED_CAM_3_DDR DDRA
	#define LED_CAM_3_PORT PORTA
	#define LED_CAM_3 6

	#define LED_CAM_4_DDR DDRD
	#define LED_CAM_4_PORT PORTD
	#define LED_CAM_4 3

	#define LED_STORE_DDR DDRA
	#define LED_STORE_PORT PORTA
	#define LED_STORE_1 2
	#define LED_STORE_2 3
	#define LED_STORE_3 2
	#define LED_STORE_4 3

void hardware_init(void)
{
	SOFTKEY_DDR &= ~((1<<SOFTKEY_1) |  (1<<SOFTKEY_2) | (1<<SOFTKEY_3) | (1<<SOFTKEY_4));
	SOFTKEY_PORT= (1<<SOFTKEY_1) |  (1<<SOFTKEY_2) | (1<<SOFTKEY_3) | (1<<SOFTKEY_4);

	CAMKEY_DDR &=~((1<<CAMKEY_1) | (1<<CAMKEY_2) | (1<<CAMKEY_3) | (1<<CAMKEY_4)); 
	CAMKEY_PORT |= (1<<CAMKEY_1) | (1<<CAMKEY_2) | (1<<CAMKEY_3) | (1<<CAMKEY_4);

	STOREKEY_DDR &=~((1<<STORE_1) | (1<<STORE_2) | (1<<STORE_3) | (1<<STORE_4)); 
	STOREKEY_PORT |= (1<<STORE_1) | (1<<STORE_2) | (1<<STORE_3) | (1<<STORE_4);

	LED_STORE_DDR  |= (1<<LED_STORE_1) | (1<<LED_STORE_2) | (1<<LED_STORE_3) | (1<<LED_STORE_4); 
	LED_STORE_PORT |= (1<<LED_STORE_1) | (1<<LED_STORE_2) | (1<<LED_STORE_3) | (1<<LED_STORE_4);


	LED_CAM_1_DDR |= (1<<LED_CAM_1);
	LED_CAM_2_DDR |= (1<<LED_CAM_2);
	LED_CAM_3_DDR |= (1<<LED_CAM_3);
	LED_CAM_4_DDR |= (1<<LED_CAM_4);


}

camkey_t get_camkeys(void)
{	
	static uint8_t old_k1=0;
//static uint8_t old_k2=0;
//	static uint8_t old_k3=0;
//	static uint8_t old_k4=0;


	if( (CAMKEY_PIN & (1<<CAMKEY_1)) )
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

	if( (CAMKEY_PIN & (1<<CAMKEY_2)))
		return CAM2;
	if( (CAMKEY_PIN & (1<<CAMKEY_3)))
		return CAM3;
	if( (CAMKEY_PIN & (1<<CAMKEY_4)))
		return CAM4;
	return CAM_NO_KEY;
}

softkey_t get_softkeys(void)
{	
	static uint8_t old_k1=0;
	static uint8_t old_k2=0;
	static uint8_t old_k3=0;
	static uint8_t old_k4=0;


	if( (SOFTKEY_PIN & (1<<SOFTKEY_1)))
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

	if( (SOFTKEY_PIN & (1<<SOFTKEY_2)))
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


	if( (SOFTKEY_PIN & (1<<SOFTKEY_3)))
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


	if( (SOFTKEY_PIN & (1<<SOFTKEY_4)))
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

	if( (STOREKEY_PIN & (1<<STORE_1)))
		return STORE1;
	if( (STOREKEY_PIN & (1<<STORE_2)))
		return STORE2;
	if( (STOREKEY_PIN & (1<<STORE_3)))
		return STORE3;
	if( (STOREKEY_PIN & (1<<STORE_4)))
		return STORE4;
	return STORE_NO_KEY;
}

rotselkey_t get_rotselkeys(void)
{
	return ROTSEL1;
}

void set_store_led(uint8_t led)
{
	switch(led)
	{
		case 0: LED_STORE_PORT &=~(1<<LED_STORE_1); break;
		case 1: LED_STORE_PORT &=~(1<<LED_STORE_2); break;
		case 2: LED_STORE_PORT &=~(1<<LED_STORE_3); break;
		case 3: LED_STORE_PORT &=~(1<<LED_STORE_4); break;
	}	
}

void reset_store_led(uint8_t led)
{
	switch(led)
	{
		case 0: LED_STORE_PORT |= (1<<LED_STORE_1); break;
		case 1: LED_STORE_PORT |= (1<<LED_STORE_2); break;
		case 2: LED_STORE_PORT |= (1<<LED_STORE_3); break;
		case 3: LED_STORE_PORT |= (1<<LED_STORE_4); break;
	}	

}

void set_cam_leds(uint8_t active)
{
	switch(active)
	{
		case 0: LED_CAM_1_PORT &=~(1<<LED_CAM_1);
				LED_CAM_2_PORT |= (1<<LED_CAM_2);
				LED_CAM_3_PORT |= (1<<LED_CAM_3);
				LED_CAM_4_PORT |= (1<<LED_CAM_4);
				break;
		case 1: LED_CAM_1_PORT |= (1<<LED_CAM_1);
				LED_CAM_2_PORT &=~(1<<LED_CAM_2);
				LED_CAM_3_PORT |= (1<<LED_CAM_3);
				LED_CAM_4_PORT |= (1<<LED_CAM_4);
				break;
		case 2: LED_CAM_1_PORT |= (1<<LED_CAM_1);
				LED_CAM_2_PORT |= (1<<LED_CAM_2);
				LED_CAM_3_PORT &=~(1<<LED_CAM_3);
				LED_CAM_4_PORT |= (1<<LED_CAM_4);
				break;
		case 3: LED_CAM_1_PORT |= (1<<LED_CAM_1);
				LED_CAM_2_PORT |= (1<<LED_CAM_2);
				LED_CAM_3_PORT |= (1<<LED_CAM_3);
				LED_CAM_4_PORT &=~(1<<LED_CAM_4);
				break;
	}
}

void set_rotarys_leds(uint8_t number)
{


}


