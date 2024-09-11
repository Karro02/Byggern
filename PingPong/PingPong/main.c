/*
 * PingPong.c
 *
 * Created: 04.09.2024 09:22:10
 * Author : karolaol
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#define FOSC 4915200// Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

#define BASE_ADDRESS 0x1000
#define SRAM_OFFSET 0x0800

void SRAM_test(void)
{
	volatile char *ext_ram = (char *) 0x1800; // Start address for the SRAM
	uint16_t ext_ram_size = 0x800;
	uint16_t write_errors = 0;
	uint16_t retrieval_errors = 0;
	printf("Starting SRAM test...\n");
	// rand() stores some internal state, so calling this function in a loop will
	// yield different seeds each time (unless srand() is called before this function)
	uint16_t seed = rand();
	// Write phase: Immediately check that the correct value was stored
	srand(seed);
	for (uint16_t i = 0; i < ext_ram_size; i++) {
		uint8_t some_value = rand();
		ext_ram[i] = some_value;
		uint8_t retreived_value = ext_ram[i];
		if (retreived_value != some_value) {
			printf("Write phase error: ext_ram[%4d] = %02X (should be %02X)\n", i, retreived_value, some_value);
			write_errors++;
		}
	}
	// Retrieval phase: Check that no values were changed during or after the write phase
	srand(seed);
	// reset the PRNG to the state it had before the write phase
	for (uint16_t i = 0; i < ext_ram_size; i++) {
		uint8_t some_value = rand();
		uint8_t retreived_value = ext_ram[i];
		if (retreived_value != some_value) {
			printf("Retrieval phase error: ext_ram[%4d] = %02X (should be %02X)\n", i, retreived_value, some_value);
			retrieval_errors++;
		}
	}
	printf("SRAM test completed with \n%4d errors in write phase and \n%4d errors in retrieval phase\n\n", write_errors, retrieval_errors);
}




void USART_Init(unsigned int ubrr) { //For uart0
	//set baud rate
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<URSEL0)|(0<<USBS0)|(3<<UCSZ00); 
}

void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

unsigned char USART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );
	/* Get and return received data from buffer */
	return UDR0;
} 
//Maybe implement if else to return only the thing in the register

int uart_putchar(char c, FILE *stream) {
	if (c == '\n') {
		USART_Transmit('\r');  // Send carriage return before newline
	}
	USART_Transmit(c);
	return 0;
}

// Custom receive function (optional, for scanf or similar)
int uart_getchar(FILE *stream) {
	return USART_Receive();
}

void printf_init() {
	fdevopen(uart_putchar, uart_getchar); //kan trenge referanse til funksjonen
}
void xmem_init() {
	MCUCR |= 1 << SRE; // enable external memory interface
	SFIOR &= ~(1 << XMM1 | 1 << XMM0); //setting XMM1 and XMM0 to 0 Worked with XMM2 instead of XMM0
	SFIOR |= 1 << XMM2; // masking out the bits that JTAG uses Set other ones to 0
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

int main(void)
{	
	USART_Init(MYUBRR);
	printf_init();

	printf("UART initialized!\n");
	
	xmem_init();
	SRAM_test();
	
	//printf("%4d", xmem_read(0x0832));
	//xmem_write(0x11, 0x1);
	//printf("%4d", xmem_read(10));
	//xmem_write(0, 0);
	
	//DDRA = 1 << PA0;
	//DDRE = 1 << PE1;
	while (1) {
		char r = USART_Receive();
		if (r == 's') {
			xmem_write(1, SRAM_OFFSET - 1);
			//printf("%4d", xmem_read(0x1));
		}
		if (r == 'd') {
			xmem_write(1, SRAM_OFFSET + 1);
		}
		
	}
	while(0) {
		char r = USART_Receive();
		if (r == 'l') {
			USART_Transmit('l');
			PORTE = 1 << PE1;
		} 
		if (r == 's') {
			USART_Transmit('s');
			PORTE = 0 << PE1;
		}
		if (r == 'd') {
			PORTA = 1;
			USART_Transmit('d');
		}
		if (r == 'k') {
			PORTA = 0;
			USART_Transmit('k');
		}
	}

	
	
	
    /* Replace with your application code */
	/*int sleep = 0;
    while (1) 
    {
		if (sleep > 1000000) {
			sleep = 0;
		}
		else if ((sleep / 1000) % 2 == 0) {
			PORTA = 0;
		} 
		else {
			PORTA = 1;
		}
		
		sleep++;
    }*/
	
	
	
	return 0;
		}

