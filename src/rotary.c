#include <avr/io.h>
#include <stdint.h>
#include <stddef.h>
#include "rotary.h"

#define ROTARY_DDR DDRA
#define ROTARY_PORT PORTA
#define ROTARY_PIN PINA

#define ROTARY_A 5
#define ROTARY_B 4


#define PHASE_A     (ROTARY_PIN & 1<<ROTARY_A)
#define PHASE_B     (ROTARY_PIN & 1<<ROTARY_B)

static int8_t encode_read( void );


static int8_t enc_delta;          // -128 ... 127
static int8_t last;
static rotary_config_t* current_config = NULL;

void rotary_init(void)
{
	ROTARY_DDR &= ~( (1<<ROTARY_A) | (1<<ROTARY_B));

	int8_t new;

	new = 0;
	if( PHASE_A ) new = 3;
	if( PHASE_B ) new ^= 1;       // convert gray to binary
	last = new;                   // power on state
	enc_delta = 0;
}

void rotary_tick(void)
{
	int8_t new, diff;

	new = 0;
	if( PHASE_A ) new = 3;
	if( PHASE_B ) new ^= 1;          // convert gray to binary
	diff = last - new;               // difference last - new
	if( diff & 1 ) 
	{                 // bit 0 = value (1)
		last = new;                    // store new as next last
		enc_delta += (diff & 2) - 1;   // bit 1 = direction (+/-)
	}
}

void rotary_setconfig(rotary_config_t* config)
{
	current_config = config;

}

void rotary_process(void)
{
	int8_t diff = encode_read();
	int32_t tmp;

	if(diff!=0 && current_config!=NULL)
	{
		diff = diff * current_config->multi;

		switch(current_config->type)
		{
			case rotary_uint8: tmp = *(current_config->data_u8)+(int16_t)diff; 

								if( diff>0  && tmp > current_config->max )
								{
									tmp = current_config->max;
								}
								else if( diff < 0 &&  tmp < (int16_t)current_config->min )
								{
									tmp = current_config->min;
								}

								*(current_config->data_u8)=tmp;

			break;
			case rotary_uint16: tmp = *(current_config->data_u16)+(int32_t)diff; 

								if( diff>0  && tmp > current_config->max )
								{
									tmp = current_config->max;
								}
								else if( diff < 0 &&  tmp < (int32_t)current_config->min )
								{
									tmp = current_config->min;
								}

								*(current_config->data_u16)=tmp;

			break;
		}		

		if(current_config->change)
			current_config->change();
	}
}

static int8_t encode_read( void )         // read single step encoders
{
  int8_t val;
  val = enc_delta;
  enc_delta = val & 3;

  return val >> 2;
}



