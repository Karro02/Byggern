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
	
	uart_init(F_CPU, BAUDRATE);
	printf("HEI pa verden\n");
	init_can();
	servo_init();
	adc_init();
	motor_init();
	encoder_init();
	solenoid_init();
	timer_init();
	
	int a = 1;
	CanMsg start;
	while(1) {
		
		if (can_rx(&start)) { //kjøre når vi får en melding
			runGame(2);
			a = 0;
		}
	}
	return 0;
}