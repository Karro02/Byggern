/*
 * SamSam.c
 *
 * Created: 16.10.2024 13:41:22
 * Author : karolaol
 */ 


#include "sam.h"
#include "uart.h"
#include "can.h"
#include "game_time.h"

int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	
// 	REG_PMC_PCER0 |= (1 << ID_PIOB);
// 	REG_PIOB_PER = (1 << 13);
// 	REG_PIOB_OER = (1 << 13);
	
	uart_init(F_CPU, BAUDRATE);
	printf("HEI pa verden\n");
	init_can();
	servo_init();
	adc_init();
	motor_init();
	encoder_init();
	solenoid_init();
	
	int a = 1;
	while(1) {
		
		if (a) { //kjøre denne når vi får en medling
			runGame(2);
			a = 0;
		}
	}


	return 0;
}


void oldTests() {
	/* Replace with your application code */
	CanMsg test;
	// 	while(1) {
	// 		CAN0_Handler();
	// 	}
	CanMsg response = {1, 2, {10, 3}};
	can_printmsg(response);
	can_tx(response);
	while(0) {
		//printf("asdf\n");
		can_tx(response);
		//printf("sfd\n");
		//uint8_t data = (uint8_t) (*(uint32_t*)REG_CAN0_SR >> 24);
		//printf("0x%02X\n", data);
		//printf("%02x\n", data);
		//printf("0x%02X \n"*(uint32_t*)REG_CAN0_SR);
	}
	
	
	int sleep = 0;
	// 	while(!can_rx(&test)) {
	// 		if (sleep > 2000000) {
	// 			printf("ingeting\n");
	// // 			printf("BOFF: %d\n", *(REG_CAN0_SR + CAN_SR_BOFF));
	// // 			printf("ERRA: %d\n", *(REG_CAN0_SR + CAN_SR_ERRA));
	// // 			printf("ERRP: %d\n", *(REG_CAN0_SR + CAN_SR_ERRP));
	// 			sleep = 0;
	// 		}
	// 		sleep++;
	// 	}
	//printf("sdafasdf");
	can_printmsg(test);
	printf("%s\n", test.byte);
	
	
	
	while(0)
	{
		if (can_rx(&test))
		{
			SignedTuple a = getJoystickPos(test);
			printf("X: %d Y: %d\n", a.X, a.Y);
			//can_print_signed_msg(test);
		}
	}
	while (0)
	{
		int sleep = 0;
		while (sleep < 1000){sleep++;}
		//sleep = 0;
		//REG_PIOB_SODR = (1 << 13);
		//while (sleep < 1000){sleep++;}
		//REG_PIOB_CODR = (1 << 13);
	}
}