#ifndef __MCP2515_H
#define __MCP2515_H

#include <stdio.h>


typedef enum{
	TX0 = 0,
	TX1 = 1,
	TX2 = 2
	} TXBUFFER;

typedef enum{
	RX0 = 0,
	RX1 = 1
} RXBUFFER;

typedef struct{
	uint16_t id;
	char data_length;
	char data[8];
	} CAN_message;


uint8_t mcp2515_init();
void mcp2515_reset();
uint8_t mcp2515_set_mode(uint8_t mode);

uint8_t mcp2515_read (uint8_t address);
void mcp2515_write(uint8_t address, uint8_t data);

void mcp2515_request_to_send(TXBUFFER buffer);
uint8_t mcp2515_read_status();
void mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data);

void mcp2515_load_TX(TXBUFFER buffer, uint8_t data, uint16_t id);
uint8_t mcp2515_read_RX(RXBUFFER buffer);

//Sende mer generell data Pass p� at lengde p� melding matcher og at id er riktig. Maks lengde er 8 byte
void mcp2515_load_mult_TX(TXBUFFER buffer, CAN_message message);
CAN_message mcp2515_read_mult_RX(RXBUFFER buffer);

void mcp2515_test_loopBack();

#define CAN_CS PB4

/*
mcp2515.h

This file contains constants that are specific to the MCP2515.

Version     Date        Description
----------------------------------------------------------------------
v1.00       2003/12/11  Initial release

Copyright 2003 Kimberly Otten Software Consulting
*/

// Define MCP2515 register addresses

#define MCP_RXF0SIDH	0x00
#define MCP_RXF0SIDL	0x01
#define MCP_RXF0EID8	0x02
#define MCP_RXF0EID0	0x03
#define MCP_RXF1SIDH	0x04
#define MCP_RXF1SIDL	0x05
#define MCP_RXF1EID8	0x06
#define MCP_RXF1EID0	0x07
#define MCP_RXF2SIDH	0x08
#define MCP_RXF2SIDL	0x09
#define MCP_RXF2EID8	0x0A
#define MCP_RXF2EID0	0x0B
#define MCP_CANSTAT		0x0E
#define MCP_CANCTRL		0x0F
#define MCP_RXF3SIDH	0x10
#define MCP_RXF3SIDL	0x11
#define MCP_RXF3EID8	0x12
#define MCP_RXF3EID0	0x13
#define MCP_RXF4SIDH	0x14
#define MCP_RXF4SIDL	0x15
#define MCP_RXF4EID8	0x16
#define MCP_RXF4EID0	0x17
#define MCP_RXF5SIDH	0x18
#define MCP_RXF5SIDL	0x19
#define MCP_RXF5EID8	0x1A
#define MCP_RXF5EID0	0x1B
#define MCP_TEC			0x1C
#define MCP_REC			0x1D
#define MCP_RXM0SIDH	0x20
#define MCP_RXM0SIDL	0x21
#define MCP_RXM0EID8	0x22
#define MCP_RXM0EID0	0x23
#define MCP_RXM1SIDH	0x24
#define MCP_RXM1SIDL	0x25
#define MCP_RXM1EID8	0x26
#define MCP_RXM1EID0	0x27
#define MCP_CNF3		0x28
#define MCP_CNF2		0x29
#define MCP_CNF1		0x2A
#define MCP_CANINTE		0x2B
#define MCP_CANINTF		0x2C
#define MCP_EFLG		0x2D
#define MCP_TXB0CTRL	0x30
#define MCP_TXB1CTRL	0x40
#define MCP_TXB2CTRL	0x50
#define MCP_RXB0CTRL	0x60
#define MCP_RXB0SIDH	0x61
#define MCP_RXB1CTRL	0x70
#define MCP_RXB1SIDH	0x71


#define MCP_TX_INT		0x1C		// Enable all transmit interrupts
#define MCP_TX01_INT	0x0C		// Enable TXB0 and TXB1 interrupts
#define MCP_RX_INT		0x03		// Enable receive interrupts
#define MCP_NO_INT		0x00		// Disable all interrupts

#define MCP_TX01_MASK	0x14
#define MCP_TX_MASK		0x54

// Define SPI Instruction Set

#define MCP_WRITE		0x02

#define MCP_READ		0x03

#define MCP_BITMOD		0x05

#define MCP_LOAD_TX0	0x40
#define MCP_LOAD_TX1	0x42
#define MCP_LOAD_TX2	0x44

#define MCP_RTS_TX0		0x81
#define MCP_RTS_TX1		0x82
#define MCP_RTS_TX2		0x84
#define MCP_RTS_ALL		0x87

#define MCP_READ_RX0	0x90
#define MCP_READ_RX1	0x94

#define MCP_READ_STATUS	0xA0

#define MCP_RX_STATUS	0xB0

#define MCP_RESET		0xC0


// CANCTRL Register Values

#define MODE_NORMAL     0x00
#define MODE_SLEEP      0x20
#define MODE_LOOPBACK   0x40
#define MODE_LISTENONLY 0x60
#define MODE_CONFIG     0x80
#define MODE_POWERUP	0xE0
#define MODE_MASK		0xE0
#define ABORT_TX        0x10
#define MODE_ONESHOT	0x08
#define CLKOUT_ENABLE	0x04
#define CLKOUT_DISABLE	0x00
#define CLKOUT_PS1		0x00
#define CLKOUT_PS2		0x01
#define CLKOUT_PS4		0x02
#define CLKOUT_PS8		0x03


// CNF1 Register Values

#define SJW_1            0x00
#define SJW_2            0x40
#define SJW_3            0x80
#define SJW_4            0xC0

//BRP_I = 0x0I
#define BRP_2			 0x02
#define BRP_3			 0x03


// CNF2 Register Values

#define BTLMODE			0x80
#define SAMPLE_1X       0x00
#define SAMPLE_3X       0x40

#define PS1_1			0x00
#define PS1_2			0x0f
#define PS1_3			0x10
#define PS1_4			0x1f
#define PS1_5			0x20
#define PS1_6			0x2f
#define PS1_7			0x30
#define PS1_8			0x3f

#define PROPSEG_1		0x00
#define PROPSEG_2		0x01
#define PROPSEG_3		0x02
#define PROPSEG_4		0x03
#define PROPSEG_5		0x04
#define PROPSEG_6		0x05
#define PROPSEG_7		0x06
#define PROPSEG_8		0x07


// CNF3 Register Values

#define SOF_ENABLE		0x80
#define SOF_DISABLE		0x00
#define WAKFIL_ENABLE	0x40
#define WAKFIL_DISABLE	0x00

//Min value for PS2 = 2 Tq
#define PS2_2			0x01
#define PS2_3			0x02
#define PS2_4			0x03
#define PS2_5			0x04
#define PS2_6			0x05
#define PS2_7			0x06
#define PS2_8			0x07



// CANINTF Register Bits

#define MCP_RX0IF		0x01
#define MCP_RX1IF		0x02
#define MCP_TX0IF		0x04
#define MCP_TX1IF		0x08
#define MCP_TX2IF		0x10
#define MCP_ERRIF		0x20
#define MCP_WAKIF		0x40
#define MCP_MERRF		0x80






#endif