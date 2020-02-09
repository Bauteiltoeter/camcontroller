#ifndef __DMX_H
#define __DMX_H

#define DMX_BAUD 250000

#define DMX_BAUD_BREAK 80000


void write_dmx(uint16_t channel, uint8_t value);
void dmx_init(void);	


extern volatile unsigned char dmx_buffer[512];

#endif
