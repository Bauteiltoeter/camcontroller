#include "artnet.h"
#include <stdint.h>
#include "uart.h"
#include "dmx.h"
void artnet_init(void)
{
    //we do not need to initialise uart2 because it's initialized in tally.c
}

void artnet_tick(void)
{
    static uint8_t i=0;

    if(i==200)
    {
        i=0;
        uart2_putc(0xAA);
        uart2_putc(0xBB);
        for(uint8_t i=0; i < 64; i++)
        {
            uart2_putc(dmx_buffer[i]);
        }
    }
    i++;

}
