#ifndef __MAX7221_H
#define __MAX7221_H

#include <stdint.h>

void max7221_init(void);
void max7221_set_intensity(uint8_t intensity);
void max7221_set_led(uint8_t anode, uint8_t cathode);
void max7221_reset_led(uint8_t anode, uint8_t cathode);
void max7221_set_ledbuffer(uint8_t anode, uint8_t cathode);
void max7221_reset_ledbuffer(uint8_t anode, uint8_t cathode);
void max7221_send_buffer(void);
void max7221_set_digit(uint8_t cathode, uint8_t data);
#endif
