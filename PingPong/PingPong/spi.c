#include "spi.h"
#include <avr/io.h>
#include "mcp2515.h"

void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	//DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK);
	DDRB |= (1 << DDB4) | (1 << DDB5) | (1 << DDB7);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	PORTB |= 1 << PB4;
}
void SPI_MasterTransmit(char cData)
{	
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
}

void SPI_MasterWrite(char cData) {
	SPI_MasterTransmit(cData);
}
uint8_t SPI_MasterRead() {
	SPI_MasterTransmit(0b00000000);  //send dummy bits
	return SPDR;
}
