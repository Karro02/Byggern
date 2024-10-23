
#include "game_time.h"

SignedTuple getJoystickPos(CanMsg m) {
	//SignedTuple a = {m.byte[0], m.byte[1]};
	return (SignedTuple){m.byte[0], m.byte[1]};
}
