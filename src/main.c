#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <util/delay.h>
#include "lcd.h"
#include "dmx.h"
#include "adc.h"

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
#define LED_STORE_3 4
#define LED_STORE_4 5


typedef enum {
	MENU_SPLASH =0,
	MENU_MAIN = 1,
	MENU_SETUP = 2,
	MENU_EDIT_CAM = 3,
	MENU_STORE = 4,
	MENU_CLEAR = 5,
	MENU_INVALID = 255
} menu_identifiers;

typedef void(*menu_button)(void);
typedef void(*init_function)(void);

typedef struct
{
	char* lines[4];
	menu_identifiers next[4];
	menu_button cb[4];
	init_function init;
	
} menu_t;

void setup_cam_up(void);
void setup_cam_down(void);
void setup_show_cam(void);
void param_next(void);
void param_up(void);
void param_down(void);
void param_show(void);
void param_resetId(void);
void main_show(void);
void store_clear(void);
void save_data(void);

menu_t menues[] =
{ 	
	{ //MENU_SPLASH
		.lines = { "DragonVideo        ","By Karrn            ","Cam controller      ","                2017"},
		.next =  { MENU_MAIN,MENU_MAIN,MENU_MAIN,MENU_MAIN},
		.cb =    { NULL,NULL,NULL,NULL},
		.init =  NULL
    },
	{ //MENU_MAIN
		.lines = { "DragonVideo        ","                    ","                    ","STORE          SETUP"},
		.next  = { MENU_STORE, MENU_INVALID,MENU_INVALID,MENU_SETUP},
		.cb    = { NULL,NULL,NULL,NULL},
		.init  = main_show
	},
	{ //MENU_SETUP
		.lines = { "Setup              ","                    ","                    ","PREV NEXT MOD.  BACK"},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_EDIT_CAM,MENU_MAIN},
		.cb    = { setup_cam_down,setup_cam_up,param_resetId,save_data},
		.init =  setup_show_cam
	},
	{ //MENU_EDIT_CAM
		.lines = { "Setup              ","                    ","                    ","NEXT UP   DOWN  BACK"},
		.next  = { MENU_INVALID,MENU_INVALID,MENU_INVALID,MENU_SETUP},
		.cb    = { param_next,param_up, param_down, NULL},
		.init  = param_show
	},
	{ //MENU_STORE
		.lines = { "Store              ","Choose store to save","                    ","     CLEAR     ABORT"},
		.next  = { MENU_INVALID,MENU_CLEAR,MENU_INVALID,MENU_MAIN},
		.cb    = { NULL,NULL, NULL, NULL},
		.init  = NULL
	},
	{ //MENU_CLEAR
		.lines = { "Clear store        ","Choose store        ","                    ","ALL            ABORT"},
		.next  = { MENU_MAIN,MENU_INVALID,MENU_INVALID,MENU_MAIN},
		.cb    = { store_clear,NULL, NULL, NULL},
		.init  = NULL
	}
};



menu_identifiers active_menu;
void set_menu(menu_identifiers menu);
void process_menu(void);
void process_inputs(void);

typedef enum {
	NO_KEY=255,
	SK1=0,
	SK2=1,
	SK3=2,
	SK4=3
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

#define PARAM_COUNT 5
#define CAM_COUNT 4
#define STORE_COUNT 4
typedef struct  {
	uint16_t base_addr;
	uint16_t pan_address;
	uint16_t tilt_address;
	uint16_t pan_invert;
	uint16_t tilt_invert;
	uint8_t pan;
	uint8_t tilt;

	uint16_t store_pan[STORE_COUNT];
	uint16_t store_tilt[STORE_COUNT];
} cam_data_t;

cam_data_t cams[CAM_COUNT];
cam_data_t backup_cams[CAM_COUNT] EEMEM;


uint8_t active_cam;

softkey_t get_softkeys(void);
camkey_t get_camkeys(void);
storekey_t get_storekey(void);


//Hauptprogramm
int main (void) 
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

	eeprom_read_block (cams, backup_cams, sizeof(cams));

	lcd_init(LCD_DISP_ON);
	set_menu(MENU_SPLASH);
	dmx_init();
	ADC_Init();

	uint16_t blink_counter=0;
	while(1)
	{
		blink_counter++;
		process_inputs();
		process_menu();
		_delay_ms(5);

		if((active_menu == MENU_STORE || active_menu == MENU_CLEAR) && blink_counter % 20 == 0)
		{
			LED_STORE_PORT |=(1<<LED_STORE_1) | (1<<LED_STORE_2) | (1<<LED_STORE_3) | (1<<LED_STORE_4);

		}

		if((active_menu == MENU_STORE || active_menu == MENU_CLEAR )&& blink_counter % 40 == 0)
		{
			LED_STORE_PORT &=~((1<<LED_STORE_1) | (1<<LED_STORE_2) | (1<<LED_STORE_3) | (1<<LED_STORE_4));
		}
	}
}

camkey_t get_camkeys(void)
{	
	static uint8_t old_k1=0;
	static uint8_t old_k2=0;
	static uint8_t old_k3=0;
	static uint8_t old_k4=0;


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
		old_k1=0;
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
		old_k2=0;
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
		old_k3=0;
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
		old_k4=0;
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

void set_store_leds(void)
{
	if(active_menu == MENU_STORE || active_menu == MENU_CLEAR)
		return;
	
	if(cams[active_cam].store_pan[0]!= 0xFFFF && cams[active_cam].store_tilt[0]!= 0xFFFF)
	{
		LED_STORE_PORT &=~(1<<LED_STORE_1);
	}
	else
	{
		LED_STORE_PORT |= (1<<LED_STORE_1);
	}

	if(cams[active_cam].store_pan[1]!= 0xFFFF && cams[active_cam].store_tilt[1]!= 0xFFFF)
	{
		LED_STORE_PORT &=~(1<<LED_STORE_2);
	}
	else
	{
		LED_STORE_PORT |= (1<<LED_STORE_2);
	}



	if(cams[active_cam].store_pan[2]!= 0xFFFF && cams[active_cam].store_tilt[2]!= 0xFFFF)
	{
		LED_STORE_PORT &=~(1<<LED_STORE_3);
	}
	else
	{
		LED_STORE_PORT |= (1<<LED_STORE_3);
	}



	if(cams[active_cam].store_pan[3]!= 0xFFFF && cams[active_cam].store_tilt[3]!= 0xFFFF)
	{
		LED_STORE_PORT &=~(1<<LED_STORE_4);
	}
	else
	{
		LED_STORE_PORT |= (1<<LED_STORE_4);
	}
	

}

void process_inputs(void)
{
	camkey_t keys = get_camkeys();

	switch(keys)
	{
		case CAM1: active_cam = 0; break;
		case CAM2: active_cam = 1; break;
		case CAM3: active_cam = 2; break;
		case CAM4: active_cam = 3; break;
		default: break;
	}


	set_cam_leds(active_cam);
	set_store_leds();

	if(keys != NO_KEY)
	{
		if(active_menu == MENU_MAIN)
			main_show();

	}
	
	int16_t diff = axis_offset(0);

	if(cams[active_cam].pan_invert)
	{
		diff=-diff;
	}

	uint8_t old_pan = cams[active_cam].pan;
	if( (int16_t)cams[active_cam].pan+diff > 255)
		cams[active_cam].pan=255;
	else if( (int16_t)cams[active_cam].pan+diff < 0)
		cams[active_cam].pan=0;
	else
		cams[active_cam].pan+=diff	;

	diff = -axis_offset(1);

	if(cams[active_cam].tilt_invert)
	{
		diff=-diff;
	}

	uint8_t old_tilt = cams[active_cam].tilt;
	if( (int16_t)cams[active_cam].tilt+diff > 255)
		cams[active_cam].tilt=255;
	else if( (int16_t)cams[active_cam].tilt+diff < 0)
		cams[active_cam].tilt=0;
	else
		cams[active_cam].tilt+=diff	;

	

	
	storekey_t store = get_storekeys();

	if(store != STORE_NO_KEY)
	{
		uint8_t store_id=0;

		switch(store)
		{
			case STORE1: store_id = 0; break;
			case STORE2: store_id = 1; break;
			case STORE3: store_id = 2; break;
			case STORE4: store_id = 3; break;
			default: break;
		}


		if(active_menu == MENU_STORE)
		{
			cams[active_cam].store_pan[store_id] = cams[active_cam].pan;
			cams[active_cam].store_tilt[store_id] = cams[active_cam].tilt;
			//lcd_gotoxy(0,0);
			//lcd_puts("stored values");
			save_data();
			set_menu(MENU_MAIN);
		}
		else if(active_menu == MENU_CLEAR)
		{
			cams[active_cam].store_pan[store_id]=0xFFFF;
			cams[active_cam].store_tilt[store_id]=0xFFFF;
			save_data();
		}
		else
		{
			if(cams[active_cam].store_pan[store_id]!= 0xFFFF &&  cams[active_cam].store_tilt[store_id] != 0xFFFF)
			{
				cams[active_cam].pan = cams[active_cam].store_pan[store_id]; 
				cams[active_cam].tilt = cams[active_cam].store_tilt[store_id];
				//lcd_gotoxy(0,0);
				//lcd_puts("loaded values");
				//char tmp[10];
				//itoa(store_id,tmp,10);
				//lcd_puts(tmp);
			}
		}

	}

	if(old_tilt != cams[active_cam].tilt || old_pan != cams[active_cam].pan)
	{
		write_dmx(cams[active_cam].base_addr + cams[active_cam].pan_address, cams[active_cam].pan);
		write_dmx(cams[active_cam].base_addr + cams[active_cam].tilt_address, cams[active_cam].tilt);

		if(active_menu == MENU_MAIN)
			main_show();
	}	

}

void store_clear(void)
{
	for(uint8_t i=0; i < STORE_COUNT; i++)
	{
		cams[active_cam].store_pan[i] = 0xFFFF;
		cams[active_cam].store_tilt[i] = 0xFFFF;
	}
}

void main_show(void)
{
	lcd_gotoxy(0,1);
	lcd_puts("Active: CAM ");
	char tmp[21];
	itoa(active_cam+1,tmp,10);
	lcd_gotoxy(12,1);
	lcd_puts(tmp);

	lcd_gotoxy(0,2);
	sprintf(tmp,"Pan: %3d Tilt: %3d", cams[active_cam].pan, cams[active_cam].tilt);
	lcd_puts(tmp);
}	

void process_menu(void)
{
	softkey_t button = get_softkeys();

	if(button != NO_KEY )
	{
		if(menues[active_menu].cb[button] != NULL)
				menues[active_menu].cb[button]();	

		if(menues[active_menu].next[button] != MENU_INVALID)
			set_menu(menues[active_menu].next[button]);
	
					

	}	
}

void set_menu(menu_identifiers menu)
{
	active_menu = menu;
	lcd_clrscr();
	lcd_puts(menues[menu].lines[0]);
	lcd_gotoxy(0,1);
	lcd_puts(menues[menu].lines[1]);
	lcd_gotoxy(0,2);
	lcd_puts(menues[menu].lines[2]);
	lcd_gotoxy(0,3);
	lcd_puts(menues[menu].lines[3]);

	if(menues[menu].init)
		menues[menu].init();


}

uint8_t setup_active_cam=0;
uint8_t param_id=0;
uint16_t* changing_param;

void setup_show_cam(void)
{
	lcd_gotoxy(0,1);

	char tmp[10];
	itoa(setup_active_cam+1,tmp,10);
	lcd_puts("Edit Cam ");
	lcd_gotoxy(9,1);
	lcd_puts(tmp);
}

void setup_cam_up(void)
{
	setup_active_cam++;

	if(setup_active_cam>= CAM_COUNT)
		setup_active_cam=0;

	setup_show_cam();
}

void setup_cam_down(void)
{
	setup_active_cam--;
	if(setup_active_cam>= CAM_COUNT)
		setup_active_cam= CAM_COUNT-1;

	setup_show_cam();
}

void param_next(void)
{
	param_id++;

	if(param_id>=PARAM_COUNT)
		param_id=0;

	param_show();
}

void param_up(void)
{
	(*changing_param)++;
	param_show();	
}

void param_down(void)
{
	(*changing_param)--;
	param_show();	
}

void param_show(void)
{
	lcd_gotoxy(14,0);
	lcd_puts("CAM ");
	char tmp[10];
	itoa(setup_active_cam+1,tmp,10);
	lcd_gotoxy(18,0);
	lcd_puts(tmp);


	lcd_gotoxy(0,1);
	char toDraw[20];
	switch(param_id)
	{
		case 0: lcd_puts("base_addr           "); 
				changing_param=&cams[setup_active_cam].base_addr;
		
				break;
		case 1: lcd_puts("pan offset          "); 
				changing_param=&cams[setup_active_cam].pan_address;
		
				break;
		case 2: lcd_puts("tilt offset         "); 
				changing_param=&cams[setup_active_cam].tilt_address;
				break;
		case 3: lcd_puts("invert pan          "); 
				changing_param=&cams[setup_active_cam].pan_invert;
				break;
		case 4: lcd_puts("invert tilt         "); 
				changing_param=&cams[setup_active_cam].tilt_invert;
				break;
		
	}

itoa(*changing_param,toDraw,10);
	lcd_gotoxy(0,2);
	lcd_puts("                 ");
	lcd_gotoxy(0,2);
	lcd_puts(toDraw);
}

void param_resetId(void)
{
	param_id=0;
}

void save_data(void)
{
	eeprom_write_block (cams, backup_cams, sizeof(cams));

}






