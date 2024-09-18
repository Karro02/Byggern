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
#define ADC_OFFSET 0x0400

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

typedef enum {
	LEFT = 0,
	RIGHT = 1,
	UP = 2,
	DOWN = 3,
	NEUTRAL = 4
} JOYSTICKPOS;

typedef struct {
	volatile int16_t X_joystick; 
	volatile int16_t Y_joystick;
	volatile int16_t L_slider; 
	volatile int16_t R_slider;
	} joystickAndSliderPos;

typedef struct {
	volatile int16_t X;
	volatile int16_t Y;
	} signedPos;
	
typedef struct {
	volatile int16_t L;
	volatile int16_t R;
} sliderPos;
	

//PB0 og PB1 for knapper


joystickAndSliderPos get_board_data() {
	xmem_write(1, ADC_OFFSET);
	joystickAndSliderPos data;
	data.X_joystick = (int16_t) xmem_read(ADC_OFFSET);
	data.Y_joystick =  (int16_t) xmem_read(ADC_OFFSET);
	data.L_slider = (int16_t) xmem_read(ADC_OFFSET);
	data.R_slider = (int16_t) xmem_read(ADC_OFFSET);
	return data;
}

signedPos get_stick_offset() {
	signedPos Offset;
	joystickAndSliderPos data = get_board_data();
	Offset.X = (int16_t) data.X_joystick - 128;
	Offset.Y = (int16_t) data.Y_joystick - 128;
	return Offset;
}


signedPos get_percent_pos(joystickAndSliderPos pos, signedPos offset) {
	signedPos Percent;
	if (pos.X_joystick > 128 + offset.X) {
		Percent.X = (int16_t) (((double) (pos.X_joystick - offset.X - 128)) / (128 - offset.X) * 100.0);
	} else {
		Percent.X = (int16_t) (((double) (pos.X_joystick - offset.X - 128)) / (128 + offset.X) * 100.0);
	}
	
	if (pos.Y_joystick > 128 + offset.Y) {
		Percent.Y = (int16_t) (((double) (pos.Y_joystick - offset.Y - 128)) / (128 - offset.Y) * 100.0);
		} else {
		Percent.Y = (int16_t) (((double) (pos.Y_joystick - offset.Y - 128)) / (128 + offset.Y) * 100.0);
	}
	
	return Percent;	
}

sliderPos get_slider_pos(joystickAndSliderPos pos) {  //returns in percent
	sliderPos S;
	S.R = (int16_t) (((double) (pos.R_slider)) / 255 * 100.0);
	S.L = (int16_t) (((double) (pos.L_slider)) / 255 * 100.0);
	return S;
}

JOYSTICKPOS get_discrete_direction(joystickAndSliderPos pos) {
	if (pos.X_joystick > 123 && pos.X_joystick < 133 && pos.Y_joystick > 123 && pos.Y_joystick < 133) {
		return NEUTRAL;
	}
	if (pos.X_joystick > pos.Y_joystick) {
		if (pos.X_joystick > 128) {
			return LEFT;
			} else {
			return UP;
		}
	} else {
		if (pos.Y_joystick > 128) {
			return DOWN;
		} else {
			return RIGHT;
		}
	}	
}


int main(void)
{	
	USART_Init(MYUBRR);
	printf_init();

	printf("UART initialized!\n");
	
	xmem_init();
	uint16_t data = 0;
	// SRAM_test();
	
	//printf("%4d", xmem_read(0x0832));
	//xmem_write(0x11, 0x1);
	//printf("%4d", xmem_read(10));
	//xmem_write(0, 0);
	
	//DDRA = 1 << PA0;
	//DDRE = 1 << PE1;
	
	uint32_t sleep = 0;
	while(sleep < 100000) {
		sleep++;
	}
	sleep = 0;
	signedPos offset = get_stick_offset();
	printf("X: %4d Y: %4d \n", offset.X, offset.Y);
	
	signedPos zeroOffset;
	zeroOffset.X = 0;
	zeroOffset.Y = 0;
	
	while (1)
	{
		if (sleep > 100000) {
			//signedPos P = get_stick_offset();
			signedPos P = get_percent_pos(get_board_data(), offset);
			sliderPos Ps = get_slider_pos(get_board_data());
			//joystickAndSliderPos Po = get_board_data(zeroOffset);
			printf("X: %4d Y: %4d \n", P.X, P.Y);
			//printf("L: %4d R: %4d \n", Ps.L, Ps.R);
			sleep = 0;
		}
		
		sleep++;
	}
		
	while (0) {
		char r = USART_Receive();
		if (r == 's') {
			xmem_write(1, SRAM_OFFSET - 1);
			//printf("%4d", xmem_read(0x1));
		}
		if (r == 'd') {
			xmem_write(1, SRAM_OFFSET + 1);
			printf("%4d", xmem_read(SRAM_OFFSET+1));
			printf("a\n");
		}
		
	}
	
	while (0) {
		char r = USART_Receive();
		if (r == 'w') {
			xmem_write(1, ADC_OFFSET);
			//printf("%4d", xmem_read(0x1));
		}
		if (r == 'r') {
			data = (uint16_t) xmem_read(ADC_OFFSET);
			printf("%d \n", data);
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

