#include "mcp2515.h"
#include "spi.h"

uint8_t mcp2515_init() {
	uint8_t value;
	SPI_MasterInit(); // Initialize SPI
	mcp2515_reset(); // Send reset - command
	// Self - test
	value = mcp2515_read(MCP_CANSTAT); //, &value);
	if ((value & MODE_MASK) != MODE_CONFIG) {
		printf (" MCP2515 is NOT in configuration mode after reset !\n");
		return 1;
	}
	// More initialization
	
	//Criteria
	//PropSeg + PS1 >= PS2
	//PropSeg + PS1 >= T DELAY (typically 1-2 Tq)
	//PS2 > SJW
	mcp2515_write(MCP_CNF1, (SJW_1 | BRP_4)); //SJW = 1 * Tq, BRP = 4
	mcp2515_write(MCP_CNF2, (BTLMODE | SAMPLE_1X | PS1_4 | PROPSEG_4)); //Enable manually set of PS2 in CNF3, Sample once at sample point, PS1 = PROPSEG = 4 * Tq
	mcp2515_write(MCP_CNF3, (WAKFIL_ENABLE | PS2_4)); //Enable Wake_up filter, PS2 = 4 * Tq
	
	//trenger kanskje tx og mask register
	//uint8_t current_cmd = mcp2515_read(MCP_RXB0CTRL);
	
	mcp2515_bit_modify(MCP_RXB0CTRL, 0x60, 0x20); //current_cmd | (0b01 << 5));   //Turn mask/filters off, accept all messages
	mcp2515_bit_modify(MCP_RXF0SIDL, 0xE0, 0x20);
	mcp2515_bit_modify(MCP_RXF0SIDH, 0xFF, 0x0);
	mcp2515_bit_modify(MCP_RXM0SIDL, 0xE0, 0xE0);
	mcp2515_bit_modify(MCP_RXM0SIDH, 0xFF, 0xFF);
	mcp2515_bit_modify(MCP_RXB0CTRL, 0x01, 0x0);
	
	//Enable loopBack mode
	mcp2515_set_mode(MODE_LOOPBACK);
	
	
	return 0;
}

void mcp2515_reset() {
	PORTB &= ~(1 << CAN_CS); // Select CAN - controller
	SPI_MasterWrite(MCP_RESET); // Send read instruction
	PORTB |= (1 << CAN_CS); // Deselect CAN - controller
}


uint8_t mcp2515_set_mode(uint8_t mode) {
	mcp2515_write(MCP_CANCTRL, mode);
	//Test if mode is enabled
	uint8_t value = mcp2515_read(MCP_CANSTAT); //, &value);
	if ((value & MODE_MASK) != mode) {
		printf (" MCP2515 is NOT in correct mode !\n");
		return 1;
	}
	
	return 0;
}

uint8_t mcp2515_read ( uint8_t address )
{
	uint8_t result ;
	PORTB &= ~(1 << CAN_CS); // Select CAN - controller
	SPI_MasterWrite(MCP_READ); // Send read instruction
	SPI_MasterWrite(address); // Send address
	result = SPI_MasterRead(); // Read result
	PORTB |= (1 << CAN_CS); // Deselect CAN - controller
	return result ;
}

void mcp2515_write(uint8_t address, uint8_t data) {
	PORTB &= ~(1 << CAN_CS); // Select CAN - controller
	SPI_MasterWrite(MCP_WRITE);
	SPI_MasterWrite(address);
	SPI_MasterWrite(data);
	PORTB |= (1 << CAN_CS); // Deselect CAN - controller
}

void mcp2515_request_to_send(TXBUFFER buffer) {
	PORTB &= ~(1 << CAN_CS); // Select CAN - controller
	switch (buffer) {
		case TX0:
			SPI_MasterWrite(MCP_RTS_TX0);
			break;
		case TX1:
			SPI_MasterWrite(MCP_RTS_TX1);
			break;
		case TX2:
			SPI_MasterWrite(MCP_RTS_TX2);
			break;
		//case ALL:
			//SPI_MasterWrite(MCP_RTS_ALL);
			//break;		
	} 
	int tall = mcp2515_read(MCP_TXB0CTRL); //& 0b00010000
	printf("TXREQ: %d \n", tall);
	PORTB |= (1 << CAN_CS); // Deselect CAN - controller
}
uint8_t mcp2515_read_status() {
	PORTB &= ~(1 << CAN_CS); // Select CAN - controller
	SPI_MasterWrite(MCP_READ_STATUS);
	uint8_t data = SPI_MasterRead();
	if (data != SPI_MasterRead()) {
		printf("Noe rart skjedde\n");
		
	}
	PORTB |= (1 << CAN_CS); // Deselect CAN - controller
	return data;
}


void mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data) {
	PORTB &= ~(1 << CAN_CS); // Select CAN - controller
	SPI_MasterWrite(MCP_BITMOD); // Send read instruction
	SPI_MasterWrite(address); // Send address
	SPI_MasterWrite(mask); // Send mask
	SPI_MasterWrite(data); // Send data
	PORTB |= (1 << CAN_CS); // Deselect CAN - controller
}

void tx_transmit_message(uint8_t TxBn, uint8_t data){
	//mcp2515_write(TxBn, 0x8);		//Pin control
	mcp2515_write(TxBn+0x01, 0xFF);	//SIDH (Standard identifier high)
	mcp2515_write(TxBn+0x02, 0x07);	//SIDL (Standard identifier low)
	mcp2515_write(TxBn+0x05, 0x1);	//data length in bytes
	mcp2515_write(TxBn+0x06, data);
}

void mcp2515_load_TX(TXBUFFER buffer, uint8_t data, uint16_t id) {
	//PORTB &= ~(1 << CAN_CS); // Select CAN - controller
	switch (buffer) {
		case TX0:
			//tx_transmit_message(0x30, data);
			mcp2515_write(MCP_TXB0CTRL + 0x01, id >> 3);	//SIDH (Standard identifier high)
			mcp2515_write(MCP_TXB0CTRL + 0x02, (id && 0b00000111) << 5);	//SIDL (Standard identifier low)
			PORTB &= ~(1 << CAN_CS); // Select CAN - controller
			SPI_MasterWrite(MCP_LOAD_TX0 + 1);
			break;
		case TX1:
			SPI_MasterWrite(MCP_LOAD_TX1 + 1);
			break;
		case TX2:
			SPI_MasterWrite(MCP_LOAD_TX2 + 1);
			break;
	}
	SPI_MasterWrite(data);
	PORTB |= (1 << CAN_CS); // Deselect CAN - controller
	
	//mcp2515_write(MCP_TXB0CTRL + 0x01, 0x0);
	//mcp2515_write(MCP_TXB0CTRL + 0x02, 0x20);
	
}

uint8_t rx_receive_message(uint8_t RxBn){
	mcp2515_write(RxBn+0x1, 0x8);
	mcp2515_write(RxBn+0x2, 0x20);
	mcp2515_write(RxBn+0x5, 0x8);
	PORTB &= ~(1 << CAN_CS);
	SPI_MasterWrite(0b10010010);
	uint8_t data = SPI_MasterRead();
	PORTB |= (1 << CAN_CS);
	return data;
}

uint8_t mcp2515_read_RX(RXBUFFER buffer, uint8_t id) {
	PORTB &= ~(1 << CAN_CS); // Select CAN - controller
	switch (buffer) {
		case RX0:
		SPI_MasterWrite(MCP_READ_RX0);
		break;
		case RX1:
		SPI_MasterWrite(MCP_READ_RX1);
		break;
	}
	uint8_t id_read = SPI_MasterRead();
	PORTB |= (1 << CAN_CS); // Deselect CAN - controller
	
	if (/*id_read == id*/ 1) {
		PORTB &= ~(1 << CAN_CS); // Select CAN - controller
	switch (buffer) {
		case RX0:
			SPI_MasterWrite(MCP_READ_RX0 + 0x2);
			break;
		case RX1:
			SPI_MasterWrite(MCP_READ_RX1 + 0x2);
			break;
	}
	uint8_t data = SPI_MasterRead();
	PORTB |= (1 << CAN_CS); // Deselect CAN - controller
	return data;
	}
	
	return 0;
	
	
}

void mcp2515_test_loopBack() {
	mcp2515_set_mode(MODE_LOOPBACK);
	TXBUFFER test_buffer = TX0;
	RXBUFFER read_buffer = RX0;
	//tx_transmit_message(0x30, 0xef);
	
	mcp2515_load_TX(test_buffer, 0xb4, 0b00000000001);
	
	mcp2515_request_to_send(test_buffer);
	uint8_t data = mcp2515_read_RX(read_buffer, 1);
	printf("0x%02X\n", data);
	
	mcp2515_load_TX(test_buffer, 0xca, 1);
	
	mcp2515_request_to_send(test_buffer);
	data = mcp2515_read_RX(read_buffer, 1);
	printf("0x%02X\n", data);
	
	mcp2515_load_TX(test_buffer, 0x6c, 0b00000000000);
	
	mcp2515_request_to_send(test_buffer);
	data = mcp2515_read_RX(read_buffer, 1);
	printf("0x%02X\n", data);
}