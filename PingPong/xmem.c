#include "xmem.h"

void xmem_init() {
	//SRAM
	MCUCR |= 1 << SRE; // enable external memory interface
	SFIOR &= ~(1 << XMM1 | 1 << XMM0); //setting XMM1 and XMM0 to 0 Worked with XMM2 instead of XMM0
	SFIOR |= 1 << XMM2; // masking out the bits that JTAG uses Set other ones to 0
	
	// ADC
	DDRD = 1 << DDD4; // set PD4 as PWM clock signal
	TCCR3A |=  1 << WGM30 | 0 << WGM31 | 0 << COM3A0 | 1 << COM3A1;
	TCCR3B |= 1 << WGM32 | 0 << ICES3 | 1 << CS30;
	
	//EMCUCR = 0 << SRW10 | 0 << SRW11;
	

}

void xmem_write ( uint8_t data , uint16_t addr ) {
	volatile char * ext_mem = ( char *) BASE_ADDRESS ;
	ext_mem [ addr ]= data ;
}

uint8_t xmem_read ( uint16_t addr ) {
	volatile char * ext_mem = ( char *) BASE_ADDRESS ;
	uint8_t ret_val = ext_mem [ addr ];
	return ret_val ;
}