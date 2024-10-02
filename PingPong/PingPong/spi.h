#pragma once

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

void SPI_MasterInit(void);
void SPI_MasterTransmit(char cData);
//void SPI_SlaveInit(void);
//char SPI_SlaveReceive(void);
//void SPI_init(void);