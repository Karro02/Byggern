#include "game_time.h"
#include "uart.h"
#include "sam.h"

void updateJoystickPos(CanMsg m, SignedTuple* joyPos) {
	joyPos->X = m.byte[0];
	joyPos->Y = m.byte[1];
}

int detectHit(uint16_t config) {
	uint16_t data = get_adc_data();
	if (data < 50) {
		return 1;
	}
	return 0;
}


void runGame(int lifes) {
	CanMsg JoyMSG;
	int hitFlag = 0;
	uint16_t adc_config = get_adc_data();
	SignedTuple JoyPos = {0, 0, 1};
	PID_data data = {0};
	
	//Config real time timer
	RTT->RTT_MR |= (1 << 18);
	
	while(1) {
		if (can_rx(&JoyMSG))
		{
			updateJoystickPos(JoyMSG, &JoyPos);
			int joy_btn = JoyMSG.byte[2];
			control_servo(JoyPos.Y);
			activate_solenoid(joy_btn);
			
		}
		
		if (get_time() >= PERIOD) { 
			PID_controller(read_encoder(), JoyPos.X, &data);
			//reset timer
			TC1->TC_CHANNEL[2].TC_CCR |= TC_CCR_SWTRG;
		}
		
		
		if (detectHit(adc_config) && !hitFlag) {
			lifes--;
			hitFlag = 1;
		}
		
		if (!detectHit(adc_config)) {
			hitFlag = 0;
		}
		
		if (lifes <= 0) {
			control_motor_speed(0);
			int score = RTT->RTT_VR;  //read from real time timer register
 			CanMsg game_over = {1, 1, {score}};
 			can_tx(game_over);
 			return;
		}
		
	}
}


void timer_init() {
	//enable TC5
	PMC->PMC_PCER1 |= (1 << (ID_TC5 - 32));
	
	TC1->TC_CHANNEL[2].TC_CCR |= TC_CCR_CLKEN | TC_CCR_SWTRG;
	TC1->TC_CHANNEL[2].TC_CMR |= TC_CMR_TCCLKS_TIMER_CLOCK4; //timer increment at MCK/128
}



uint32_t get_time() {
	return TC1->TC_CHANNEL[2].TC_CV;
}


void PID_controller(int encoder_pos, int ref, PID_data* Data) {
	
	float e = (-ref + 100) * ENCODER_STEPS / 200 - encoder_pos;
	
	Data->error_sum += (int32_t) e;
	
	if (Data->error_sum > MAX_ERROR_SUM) {
		Data->error_sum = MAX_ERROR_SUM;
	} else if (Data->error_sum < -MAX_ERROR_SUM) {
		Data->error_sum = -MAX_ERROR_SUM;
	}
	
	float u = K_p * e + K_i * PERIOD * (double) Data->error_sum;
	
	if(u > 100.0) {
		u = 100.0;
	} else if (u < -100.0) {
		u = -100.0;
	}
	
	control_motor_speed(-u);
}