#include "spi.h"
#include <avr/io.h>

void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	//DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK);
	DDRB |= (1 << PB5) | (1 << PB7); //(1<<DDB5)|(1<<DDB7);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0); //|(1<<CPOL);
}
void SPI_MasterTransmit(char cData)
{
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)))
	;
}

//void SPI_SlaveInit(void)
//{
	///* Set MISO output, all others input */
	//DDRB = (1<<DDB6);
	///* Enable SPI */
	//SPCR = (1<<SPE);
//}
//char SPI_SlaveReceive(void)
//{
	///* Wait for reception complete */
	//while(!(SPSR & (1<<SPIF)))
	//;
	///* Return data register */
	//return SPDR;
//}
//
//void SPI_init(void)
//{
	//SPI_MasterInit();
	//SPI_SlaveInit();
//}