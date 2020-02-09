#include "tally.h"
#include "uart.h"
#include "globals.h"
#include <avr/io.h>


#define TALLY_DDR DDRJ
#define TALLY_PORT PORTJ
#define TALLY_GREEN 7
#define TALLY_RED 4

typedef enum 
{
    off,
    red,
    green,
    yellow,
    blue
} tally_color_t;

tally_mode_t tally_mode = tally_auto;
tally_rf_status_t tally_rf_status = rf_wait;

uint8_t cam5_tally=0;

void tally_update();
void tally_sendRGB(uint8_t r, uint8_t g, uint8_t b);
void tally_process(uint8_t c);
void tally_rf_process(uint8_t c);


void tally_init()
{
    uart2_init(UART_BAUD_SELECT(9600,F_CPU) ); //UART for vMIX communication
    uart1_init(UART_BAUD_SELECT(9600,F_CPU) ); //UART for wireless module

    TALLY_DDR |= (1<<TALLY_GREEN) | (1<<TALLY_RED);

    TALLY_PORT |= (1<<TALLY_GREEN) | (1<<TALLY_RED);

    //tally_setColor(off);
}



void tally_setColor(tally_color_t color);

void tally_tick()
{
    unsigned int c = uart2_getc();
    if ( c & UART_NO_DATA )
    {
        /* 
         * no data available from UART 
         */
    }
    else
    {
        /*
         * new data available from UART
         * check for Frame or Overrun error
         */
        if ( c & UART_FRAME_ERROR )
        {
            /* Framing Error detected, i.e no stop bit detected */
            uart_puts_P("UART Frame Error: ");
        }
        if ( c & UART_OVERRUN_ERROR )
        {
            /* 
             * Overrun, a character already present in the UART UDR register was 
             * not read by the interrupt handler before the next character arrived,
             * one or more received characters have been dropped
             */
            uart_puts_P("UART Overrun Error: ");
        }
        if ( c & UART_BUFFER_OVERFLOW )
        {
            /* 
             * We are not reading the receive buffer fast enough,
             * one or more received character have been dropped 
             */
            uart_puts_P("Buffer overflow error: ");
        }
        /* 
         * send received character back
         */
        tally_process((uint8_t)c);
    }

    c = uart1_getc();
    if ( c & UART_NO_DATA )
    {
        /* 
         * no data available from UART 
         */
    }
    else
    {
        /*
         * new data available from UART
         * check for Frame or Overrun error
         */
        if ( c & UART_FRAME_ERROR )
        {
            /* Framing Error detected, i.e no stop bit detected */
            uart_puts_P("UART Frame Error: ");
        }
        if ( c & UART_OVERRUN_ERROR )
        {
            /* 
             * Overrun, a character already present in the UART UDR register was 
             * not read by the interrupt handler before the next character arrived,
             * one or more received characters have been dropped
             */
            uart_puts_P("UART Overrun Error: ");
        }
        if ( c & UART_BUFFER_OVERFLOW )
        {
            /* 
             * We are not reading the receive buffer fast enough,
             * one or more received character have been dropped 
             */
            uart_puts_P("Buffer overflow error: ");
        }
        /* 
         * send received character back
         */
        tally_rf_process((uint8_t)c);
    }
}

void tally_rf_process(uint8_t c)
{
    static uint8_t buffer[20];
    
    static uint8_t mode=0;
    static uint8_t index=0;

    if(mode ==0)
    {
        if(c == 0x02) // STX
        {
            mode = 1;
            index =0;
        }
    }
    else if ( mode == 1)
    {
        if(c == 0x03) //ETX
        {
            if(index < 2)
            {
                tally_rf_status = rf_error_com;
            }
            else
            {
                if( buffer[0] == 'O' && buffer[1] == 'K')
                {
                    tally_rf_status = rf_ok;
                }
                else if(buffer[0] = 'E')
                {
                    tally_rf_status = rf_error_neko;
                }
            }
            

            //communication finished
            mode=0;
        }   
        else
        {
            if(index < 20)
                buffer[index]=c;
            index++;
        }
             
    }
    
}

void tally_process(uint8_t c)
{
    static uint8_t mode=0;
    static uint8_t buffer[6];
    static uint8_t index=0;

    switch(mode)
    {
        case 0: 
            if(c == 0xAA)
            {
                index=0;
                mode = 1;
            }
            break;
        case 1:
            buffer[index] = c;
            index++;
            if(index > 5)
            {
                if(buffer[5]==0xAA)
                {
                    cams[0].tally = buffer[0];
                    cams[1].tally = buffer[1];
                    cams[2].tally = buffer[2];
                    cams[3].tally = buffer[3];

                    cam5_tally = buffer[4];
                    tally_update();
                }
                mode = 0;
            }


    }


}


void tally_update(void)
{
    if(tally_mode == tally_auto)
    {
        if(cam5_tally == 1 ) //program
        {
            tally_setColor(red);
        }
        else if(cam5_tally == 2) //preview
        {
            tally_setColor(green);
        }
        else 
        {
            tally_setColor(off);
        }
    }
    else if(tally_mode == tally_blue)
    {
        tally_setColor(blue);
    }
    else if(tally_mode == tally_yellow)
    {
        tally_setColor(yellow);
    }
}


void tally_setColor(tally_color_t color)
{
    switch(color)
    {
        case off: 
            TALLY_PORT &= ~(1<<TALLY_RED);
            TALLY_PORT &= ~(1<<TALLY_GREEN);
            tally_sendRGB(0x0,0x0,0x0);
        break;
        case red: 
            TALLY_PORT |= (1<<TALLY_RED);
            TALLY_PORT &= ~(1<<TALLY_GREEN);  
            tally_sendRGB(0xFF,0x0,0x0);
        break;
        case green: 
            TALLY_PORT |= (1<<TALLY_GREEN);
            TALLY_PORT &= ~(1<<TALLY_RED);  
            tally_sendRGB(0x0,0xFF,0x0);
        break;
        case yellow: 
            TALLY_PORT &= ~(1<<TALLY_RED);
            TALLY_PORT &= ~(1<<TALLY_GREEN);
            tally_sendRGB(0xFF,0xFF,0x0);
        break;
        case blue: 
            TALLY_PORT &= ~(1<<TALLY_RED);
            TALLY_PORT &= ~(1<<TALLY_GREEN);
            tally_sendRGB(0x0,0x0,0xFF);
        break;
    }
}


void tally_setAuto()
{
    tally_mode = tally_auto;
    tally_update();
}

void tally_setYellow()
{
    tally_mode = tally_yellow;
    tally_update();
}

void tally_setBlue()
{
    tally_mode = tally_blue;
    tally_update();
}


tally_mode_t tally_getMode()
{
    return tally_mode;
}


void tally_sendRGB(uint8_t r, uint8_t g, uint8_t b)
{
    //  [STX]NExxxyyyzzz[ETX]
    tally_rf_status = rf_wait;
    uart1_putc(2); //STX
    uart1_putc('N');
    uart1_putc('E');
    
    char t;

    t = '0' + r/100;
    uart1_putc(t);
    t = '0' + r % 100 / 10;
    uart1_putc(t);
    t = '0' + r %10;
    uart1_putc(t);

    t = '0' + g/100;
    uart1_putc(t);
    t = '0' + g % 100 / 10;
    uart1_putc(t);
    t = '0' + g %10;
    uart1_putc(t);

    t = '0' + b/100;
    uart1_putc(t);
    t = '0' + b % 100 / 10;
    uart1_putc(t);
    t = '0' + b %10;
    uart1_putc(t);
    
    uart1_putc(3); //ETX
}