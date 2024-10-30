#pragma once

#include "can.h"
#include "gameBoard.h"

typedef struct{
	int8_t X;
	int8_t Y;
	} SignedTuple;
	
SignedTuple getJoystickPos(CanMsg m);

int detectHit();

void runGame(int lifes);