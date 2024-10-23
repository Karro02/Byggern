#pragma once

#include "can.h"

typedef struct{
	int8_t X;
	int8_t Y;
	} SignedTuple;
	
SignedTuple getJoystickPos(CanMsg m);