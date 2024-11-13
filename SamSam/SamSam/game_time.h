#pragma once

#include "can.h"
#include "gameBoard.h"

#define MAX_ERROR_SUM 5000
#define ENCODER_STEPS 5583

// #define T (0.01 / 656250.0) //Sample period of 0.01s
// #define K_p 0.1
// #define K_i 0.05

#define PERIOD (0.01 / 656250.0)
#define K_p 0.04
#define K_i 0.02

typedef struct{
	int8_t X;
	int8_t Y;
	} SignedTuple;
	
typedef struct {
	uint32_t time;
	int32_t error_sum;
} PID_data;
	
	
void updateJoystickPos(CanMsg m, SignedTuple* joyPos);

int detectHit(uint16_t config);

void runGame(int lifes);


void timer_init();
uint32_t get_time();	//counter with f = 656 250 Hz

void PID_controller(int encoder_pos, int ref, PID_data* Data);