#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "dmx.h"


volatile unsigned char dmx_buffer[512];


void dmx_init(void)
{
		//Init usart DMX-BUS
	UBRR0   = (F_CPU / (DMX_BAUD * 16L) - 1);
	DDRD |= (1<<PD1); //Output TXD Pin ATmega88
	UCSR0B|=(1<<TXEN0)|(1<<TXCIE0); // TXEN0 Transmitter enable / TXCIE0 TX complete interrupt enable 
	UCSR0C|=(1<<USBS0); //USBS0 2 Stop bits	
	sei();//Globale Interrupts Enable
	UDR0 = 0;//Start DMX

}

void write_dmx(uint16_t channel, uint8_t value)
{
	if(channel < 512)
		dmx_buffer[channel]=value;
	
} 

//############################################################################

//DMX Senderoutine

ISR (USART0_TX_vect)


{

static unsigned int  dmx_channel_tx_count = 0;

static unsigned char dmx_tx_state = 0;



switch (dmx_tx_state)
{
	case (0):
		UBRR0   = (F_CPU / (DMX_BAUD_BREAK * 16L) - 1);
		UDR0 = 0; //RESET Frame
		dmx_tx_state = 1;
	break;

	case (1):
		UBRR0   = (F_CPU / (DMX_BAUD * 16L) - 1);
		UDR0 = 0; //Start Byte
		dmx_tx_state = 2;
	break;

	case (2):
		_delay_us(10);
		//Ausgabe des Zeichens
		UDR0 = dmx_buffer[dmx_channel_tx_count];
		dmx_channel_tx_count++;

		if(dmx_channel_tx_count == 512)
		{
			dmx_channel_tx_count = 0;
			dmx_tx_state = 0;
		}
		break;
	}
}

