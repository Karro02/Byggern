/*
 * PingPong.c
 *
 * Created: 04.09.2024 09:22:10
 * Author : karolaol
 */ 

#include <avr/io.h>
#define FOSC 4915200// Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1




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


int main(void)
{	
	USART_Init(MYUBRR);
	while(1) {
		char r = USART_Receive();
		if (r) {
			USART_Transmit(r);
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

