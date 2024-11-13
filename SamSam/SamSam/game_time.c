#include "game_time.h"
#include "uart.h"
#include "sam.h"

void updateJoystickPos(CanMsg m, SignedTuple* joyPos) {
	//SignedTuple a = {m.byte[0], m.byte[1]};
	//return (SignedTuple){m.byte[0], m.byte[1]};
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
	int sleep = 0;
	int hitFlag = 0;
	//printf("calibrate encoder %d\n", calibrate_encoder());
	uint16_t adc_config = get_adc_data();
	
	SignedTuple JoyPos = {0, 0, 1};
	
	PID_data data = {get_time(), 0};
		
	RTT->RTT_MR |= (1 << 18);
	while(1) {
		if (can_rx(&JoyMSG))
		{
			updateJoystickPos(JoyMSG, &JoyPos);
			int joy_btn = JoyMSG.byte[2];
			//printf("%d\n", joy_btn);
			control_servo(JoyPos.Y);
			//control_motor_speed(JoyPos.X);
			activate_solenoid(joy_btn);
			
		}
		
		if (get_time() >= PERIOD) { 
			PID_controller(read_encoder(), JoyPos.X, &data);
			//reset timer
			TC1->TC_CHANNEL[2].TC_CCR |= TC_CCR_SWTRG;
		}
		
		
		//printf("%d\n",  (int) ((double) get_time() / 656250.0));
		
		if (sleep > 500000) {
			//printf("adc: %d\n", get_adc_data());
			//printf("encoder: %d\n", read_encoder());
			//printf("time %d\n", get_time() / 100);
			sleep = 0;
		}
		sleep++;
		
		if (detectHit(adc_config) && !hitFlag) {  //Bør legge til et flagg her
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
 			printf("Score: %d\n", score);
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
	//printf("e: %f\n", e);
	
	Data->error_sum += (int32_t) e;
	Data->time = get_time();
	
	if (Data->error_sum > MAX_ERROR_SUM) {
		Data->error_sum = MAX_ERROR_SUM;
	} else if (Data->error_sum < -MAX_ERROR_SUM) {
		Data->error_sum = -MAX_ERROR_SUM;
	}
	
	float u = K_p * e + K_i * PERIOD * (double) Data->error_sum;
	
	//printf("u: %f\n", u);
	
	if(u > 100.0) {
		u = 100.0;
	} else if (u < -100.0) {
		u = -100.0;
	}
	
	control_motor_speed(-u);
}