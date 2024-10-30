#include "game_time.h"

SignedTuple getJoystickPos(CanMsg m) {
	//SignedTuple a = {m.byte[0], m.byte[1]};
	return (SignedTuple){m.byte[0], m.byte[1]};
}

int detectHit() {
	uint16_t data = get_adc_data();
	if (data < 1500) {
		return 1;
	}
	return 0;
}


void runGame(int lifes) {
	CanMsg JoyMSG;
	int sleep = 0;
	int hitFlag = 0;
	while(1) {
		if (can_rx(&JoyMSG))
		{
			SignedTuple JoyPos = getJoystickPos(JoyMSG);
			update_duty_cycle(JoyPos.X);
		}
		
		if (sleep > 500000) {
			printf("adc: %d\n", get_adc_data());
			sleep = 0;
		}
		sleep++;
		if (detectHit() && !hitFlag) {  //Bør legge til et flagg her
			lifes--;
			hitFlag = 1;
		}
		
		if (!detectHit()) {
			hitFlag = 0;
		}
		
		if (lifes <= 0) {
			printf("Game over /n");
			return;
		}
		
	}
	printf("Game over /n");
}