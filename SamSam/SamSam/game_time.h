#pragma once

#include "can.h"
#include "gameBoard.h"

#define MAX_ERROR_SUM 5000
#define ENCODER_STEPS 5583

#define PERIOD (0.01 / 656250.0)
#define K_p 0.04
#define K_i 0.02

typedef struct{
	int8_t X;
	int8_t Y;
	} SignedTuple;
	
typedef struct {
	int32_t error_sum;
} PID_data;
	
	
void updateJoystickPos(CanMsg m, SignedTuple* joyPos);

int detectHit(uint16_t config);

void runGame(int lifes);


void timer_init();
uint32_t get_time(); //note! not in seconds

void PID_controller(int encoder_pos, int ref, PID_data* Data);