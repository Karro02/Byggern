#include "uart.h"

void USART_Init(unsigned int ubrr) { //For uart0
	//set baud rate
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<URSEL0)|(0<<USBS0)|(3<<UCSZ00);
}

void USART_Transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

unsigned char USART_Receive(void)
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