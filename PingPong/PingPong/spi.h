#pragma once

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

void SPI_MasterInit(void);
void SPI_MasterTransmit(char cData);
void SPI_MasterWrite(char cData);
uint8_t SPI_MasterRead();
