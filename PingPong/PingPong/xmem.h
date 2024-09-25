#pragma once

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#define BASE_ADDRESS 0x1000
#define SRAM_OFFSET 0x0800
#define ADC_OFFSET 0x0400

//PB0 og PB1 for knapper

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

typedef struct {
	volatile int16_t joy_button;
	volatile int16_t l_button;
	volatile int16_t r_button;
	} buttonData;



void xmem_init();
void xmem_write (uint8_t data , uint16_t addr);
uint8_t xmem_read (uint16_t addr);

//specific functions for ADC converter. Remember to get and add offset for stick when using!!
joystickAndSliderPos get_board_data();
signedPos get_stick_offset();
signedPos get_percent_pos(joystickAndSliderPos pos, signedPos offset);
sliderPos get_slider_pos(joystickAndSliderPos pos);
JOYSTICKPOS get_discrete_direction(signedPos offset);

buttonData get_button_data();




