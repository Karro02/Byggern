/*
 * PingPong.c
 *
 * Created: 04.09.2024 09:22:10
 * Author : karolaol
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include "uart.h"
#include "xmem.h"
#include "oled.h"
#include "spi.h"
#include "fonts.h"
#include "mcp2515.h"


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

int main(void)
{	
	USART_Init(MYUBRR);
	printf_init();
	printf("UART initialized!\n");
	xmem_init();
	printf("xmem initialized! \n");
	OLED_init();
	
	if (!mcp2515_init()) {
		printf("mcpc2515 initialized! \n");
	}
	
	
	SRAM_test();
	
	
	
	
	
	//mcp2515_test_loopBack();
	
	mcp2515_set_mode(MODE_NORMAL);
	
// 	CAN_message msg = {1, 8, "helofrik"};
// 	mcp2515_load_mult_TX(TX0, msg);
// 	mcp2515_request_to_send(TX0);

	long slep = 0;
	while(1) {
		if (slep > 100000) {
 		uint8_t i = mcp2515_read(MCP_CANINTF);
 		printf("Reg: 0x%02X\n", i);
 		CAN_message msg = mcp2515_read_mult_RX(RX0);
		printf("Id: %d\n", msg.id);
		printf("%d\n", msg.data_length);
		for (int i = 0; i < msg.data_length; i++) {
			printf("%d", msg.data[i]);
		}
		printf("\n");
		if (msg.data_length > 0) {
			break;
		}
		slep = 0;
		}
		slep++;
	}
	
	while(0) {
		if (slep > 10000) {
			CAN_message msg = {1, 8, "helofrik"};
			mcp2515_load_mult_TX(TX0, msg);
			mcp2515_request_to_send(TX0);
			slep = 0;
		}
		slep++;
	}
	
	
	signedPos offset = get_stick_offset();
	printf("X: %4d Y: %4d \n", offset.X, offset.Y);
	//OLED_clear();
	OLED_run(offset);
	//OLED_home(offset);
	//OLED_sub_menu();
	
	uint8_t sleep = 0;
	while (1)
	{
		if (sleep > 100000) {
			signedPos P = get_percent_pos(get_board_data(), offset);
			sliderPos Ps = get_slider_pos(get_board_data());
			JOYSTICKPOS POS = get_discrete_direction(offset);
			buttonData PB = get_button_data();
			//printf("X: %4d Y: %4d \n", P.X, P.Y);
			//printf("L: %4d R: %4d \n", Ps.L, Ps.R);
			printf("%d \n", (int) POS);
			//printf("JOY: %4d, L: %4d, R: %4d \n", PB.joy_button, PB.l_button, PB.r_button);
			//printf("%4d \n", PINB);
			sleep = 0;
		}
		
		sleep++;
	}
		
	
	return 0;
}

