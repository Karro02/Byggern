/*
 * SamSam.c
 *
 * Created: 16.10.2024 13:41:22
 * Author : karolaol
 */ 


#include "sam.h"
#include "uart.h"
#include "can.h"

int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	
	REG_PMC_PCER0 |= (1 << ID_PIOB);
	REG_PIOB_PER = (1 << 13);
	REG_PIOB_OER = (1 << 13);
	
	uart_init(F_CPU, BAUDRATE);
	printf("HEI pa verden\n");
	init_can();
	
    /* Replace with your application code */
	CanMsg test;
	while(1) {
		CAN0_Handler();
	}
	
// 	int sleep = 0;
// 	while(!can_rx(&test)) {
// 		if (sleep > 1000000) {
// 			printf("ingeting\n");
// 			sleep = 0;
// 		}
// 		sleep++;
// 	}
	printf("sdafasdf");
	can_printmsg(test);
    while (0) 
    {
		int sleep = 0;
		while (sleep < 1000){sleep++;}
		//sleep = 0;
		//REG_PIOB_SODR = (1 << 13);
		//while (sleep < 1000){sleep++;}
		//REG_PIOB_CODR = (1 << 13);
		
		
		
		
		
    }
	return 0;
}
