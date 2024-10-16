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
	//mcp2515_write(MCP_RXB0CTRL, current_cmd | (0b11 << 5));
	//
	//current_cmd = mcp2515_read(MCP_RXB1CTRL);
	//mcp2515_write(MCP_RXB1CTRL, current_cmd | (0b11 << 5));
	
	//Godkjent ID for RX0 = 1, RX1 godkjenner alt
	mcp2515_bit_modify(MCP_RXB0CTRL, 0x60, 0x20); //current_cmd | (0b01 << 5));   //Turn mask/filters off, accept all messages
	mcp2515_bit_modify(MCP_RXF0SIDL, 0xE0, 0x20);
	mcp2515_bit_modify(MCP_RXF0SIDH, 0xFF, 0x0);
	mcp2515_bit_modify(MCP_RXM0SIDL, 0xE0, 0xE0);
	mcp2515_bit_modify(MCP_RXM0SIDH, 0xFF, 0xFF);
	mcp2515_bit_modify(MCP_RXB0CTRL, 0x01, 0x0);
	
	//mcp2515_bit_modify(MCP_RXB0CTRL, 0x60, 0x30);
	
	//Enable loopBack mode
	//mcp2515_set_mode(MODE_LOOPBACK);
	
	
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
			printf("TXREQ: 0x%02X\n", mcp2515_read(MCP_TXB0CTRL));
			break;
		case TX1:
			SPI_MasterWrite(MCP_RTS_TX1);
			printf("TXREQ: 0x%02X\n", mcp2515_read(MCP_TXB1CTRL));
			break;
		case TX2:
			SPI_MasterWrite(MCP_RTS_TX2);
			printf("TXREQ: 0x%02X\n", mcp2515_read(MCP_TXB2CTRL));
			break;
		//case ALL:
			//SPI_MasterWrite(MCP_RTS_ALL);
			//break;		
	} 
	
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

uint8_t rx_receive_message(uint8_t RxBn){
	mcp2515_write(RxBn+0x1, 0x8);
	mcp2515_write(RxBn+0x2, 0x20);
	mcp2515_write(RxBn+0x5, 0x8);
	//PORTB &= ~(1 << CAN_CS);
	//SPI_MasterWrite(0b10010010);
	//uint8_t data = SPI_MasterRead();
	uint8_t data = mcp2515_read(RxBn + 0x06);
	//PORTB |= (1 << CAN_CS);
	return data;
}

void mcp2515_load_TX(TXBUFFER buffer, uint8_t data, uint16_t id) {
	//PORTB &= ~(1 << CAN_CS); // Select CAN - controller
	switch (buffer) {
		case TX0:
			//tx_transmit_message(0x30, data);
			mcp2515_write(MCP_TXB0CTRL + 0x01, id >> 3);	//SIDH (Standard identifier high)
			mcp2515_write(MCP_TXB0CTRL + 0x02, (id && 0b00000111) << 5);	//SIDL (Standard identifier low)
			mcp2515_write(MCP_TXB0CTRL+0x05, 0x1);	//data length in bytes
			PORTB &= ~(1 << CAN_CS); // Select CAN - controller
			SPI_MasterWrite(MCP_LOAD_TX0 + 0x01);
			break;
		case TX1:
			mcp2515_write(MCP_TXB1CTRL + 0x01, id >> 3);	//SIDH (Standard identifier high)
			mcp2515_write(MCP_TXB1CTRL + 0x02, (id && 0b00000111) << 5);	//SIDL (Standard identifier low)
			mcp2515_write(MCP_TXB1CTRL+0x05, 0x1);	//data length in bytes
			PORTB &= ~(1 << CAN_CS); // Select CAN - controller
			SPI_MasterWrite(MCP_LOAD_TX1 + 0x01);
			break;
		case TX2:
			mcp2515_write(MCP_TXB2CTRL + 0x01, id >> 3);	//SIDH (Standard identifier high)
			mcp2515_write(MCP_TXB2CTRL + 0x02, (id && 0b00000111) << 5);	//SIDL (Standard identifier low)
			mcp2515_write(MCP_TXB2CTRL+0x05, 0x1);	//data length in bytes
			PORTB &= ~(1 << CAN_CS); // Select CAN - controller
			SPI_MasterWrite(MCP_LOAD_TX2 + 0x01);
			break;
	}
	SPI_MasterWrite(data);
	//SPI_MasterWrite(0x55);  //bare å write flere ganger for å skrive mer en en byte data
	PORTB |= (1 << CAN_CS); // Deselect CAN - controller
	
	//mcp2515_write(MCP_TXB0CTRL + 0x01, 0x0);
	//mcp2515_write(MCP_TXB0CTRL + 0x02, 0x20);
	
}

uint8_t mcp2515_read_RX(RXBUFFER buffer) {
	PORTB &= ~(1 << CAN_CS); // Select CAN - controller
	switch (buffer) {
		case RX0:
			SPI_MasterWrite(MCP_READ_RX0 + 0x02);
			break;
		case RX1:
			SPI_MasterWrite(MCP_READ_RX1 + 0x02);
			break;
	}
	uint8_t data = SPI_MasterRead();
	PORTB |= (1 << CAN_CS); // Deselect CAN - controller
	return data;
}

void mcp2515_load_mult_TX(TXBUFFER buffer, CAN_message message) {
	switch (buffer) {
		case TX0:
			//tx_transmit_message(0x30, data);
			mcp2515_write(MCP_TXB0CTRL + 0x01, message.id >> 3);	//SIDH (Standard identifier high)
			mcp2515_write(MCP_TXB0CTRL + 0x02, (message.id && 0b00000111) << 5);	//SIDL (Standard identifier low)
			mcp2515_write(MCP_TXB0CTRL+0x05, message.data_length);	//data length in bytes
			PORTB &= ~(1 << CAN_CS); // Select CAN - controller
			SPI_MasterWrite(MCP_LOAD_TX0 + 0x01);
			break;
		case TX1:
			mcp2515_write(MCP_TXB1CTRL + 0x01, message.id >> 3);	//SIDH (Standard identifier high)
			mcp2515_write(MCP_TXB1CTRL + 0x02, (message.id && 0b00000111) << 5);	//SIDL (Standard identifier low)
			mcp2515_write(MCP_TXB1CTRL+0x05, message.data_length);	//data length in bytes
			PORTB &= ~(1 << CAN_CS); // Select CAN - controller
			SPI_MasterWrite(MCP_LOAD_TX1 + 0x01);
			break;
		case TX2:
			mcp2515_write(MCP_TXB2CTRL + 0x01, message.id >> 3);	//SIDH (Standard identifier high)
			mcp2515_write(MCP_TXB2CTRL + 0x02, (message.id && 0b00000111) << 5);	//SIDL (Standard identifier low)
			mcp2515_write(MCP_TXB2CTRL+0x05, message.data_length);	//data length in bytes
			PORTB &= ~(1 << CAN_CS); // Select CAN - controller
			SPI_MasterWrite(MCP_LOAD_TX2 + 0x01);
			break;
	}
	
	for (int i = 0; i < message.data_length; i++) {
		SPI_MasterWrite(message.data[i]);
	}
	
	//SPI_MasterWrite(0x55);  //bare å write flere ganger for å skrive mer en en byte data
	PORTB |= (1 << CAN_CS); // Deselect CAN - controller
}
CAN_message mcp2515_read_mult_RX(RXBUFFER buffer) {
	
	uint8_t len;
	switch (buffer) {
		case RX0:
			//mcp2515_write(MCP_RXB0CTRL + 0x05, len);
			len = mcp2515_read(MCP_RXB0CTRL + 0x05); //Read from DLC
			PORTB &= ~(1 << CAN_CS); // Select CAN - controller
			SPI_MasterWrite(MCP_READ_RX0 + 0x02);
			break;
		case RX1:
			//mcp2515_write(MCP_RXB1CTRL + 0x05, len);
			len = mcp2515_read(MCP_RXB1CTRL + 0x05); //Read from DLC
			PORTB &= ~(1 << CAN_CS); // Select CAN - controller
			SPI_MasterWrite(MCP_READ_RX1 + 0x02);
			break;
	}
	CAN_message message;
	
	for(int i = 0; i < len; i++) {
		message.data[i] = SPI_MasterRead();
	}
	PORTB |= (1 << CAN_CS); // Deselect CAN - controller
// 	CAN_message message = {1, len, data};
	return message;
}

void mcp2515_test_loopBack() {
	mcp2515_set_mode(MODE_LOOPBACK);
	printf("Mode: 0x%02X\n", mcp2515_read(MCP_CANCTRL));
	TXBUFFER test_buffer = TX1;
	RXBUFFER read_buffer = RX1;
	//tx_transmit_message(0x30, 0xac);
	for (int i = 0; i < 10; i++) {
		
		mcp2515_load_TX(test_buffer, 0x00 + 2 + i, 11);
		mcp2515_request_to_send(test_buffer);
		uint8_t data = mcp2515_read_RX(read_buffer);
		//uint8_t data = rx_receive_message(0b01100000);
		printf("Data: 0x%02X\n", data);
	}
	
	/*CAN_message test = {1, 8, "abcdefgh"};*/
	CAN_message test = {11, 8, "kileraaa"};
	//CAN_message test = {11, 1, "r"};
		
	mcp2515_load_mult_TX(test_buffer, test);

	
	mcp2515_request_to_send(test_buffer);

	CAN_message test_data = mcp2515_read_mult_RX(read_buffer);
	printf("%s\n", test_data.data);
	
}