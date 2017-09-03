#ifndef __ROTARY_H
#define __ROTARY_H

typedef void(*rotary_changenote)(void);

typedef enum {
	rotary_uint8,
	rotary_uint16
} rotary_types;

typedef struct {
	uint8_t* data_u8;
	uint16_t* data_u16;
	rotary_types type;
	rotary_changenote change;
	uint16_t min;
	uint16_t max;
	uint8_t multi;
	uint8_t wrap;

} rotary_config_t;

void rotary_init(void);

void rotary_tick(void);
void rotary_process(void);
void rotary_setconfig(rotary_config_t* config);


#endif
