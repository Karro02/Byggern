#include "gameBoard.h"
#include "sam.h"

void servo_init() {
	//Enabler PWM som peripheral
	//Prøver med 1 fordi vi vil enable PWM som har id 36
	PMC->PMC_PCER1 |= (1 << 4); //PIDn = 36
	
	//Fjerner PIO kontroll på PB13
	//Bruker PWM out på PB13 som er satt ved alternate function b
	PIOB->PIO_PDR |= (1 << 13); //PIO_PDR_Pn;
	
	//Velger B funksjon(PWMH1)
	PIOB->PIO_ABSR |= (1 << 13); //PIO_ABSR_Pn;
	
	//Sette opp PWM signalet MCK = 84 MHz
	//PWM->PWM_CLK / REG_PWM_CLK;
	REG_PWM_CMR1 |= (0b0111) | (1 << 9); //Prescaler = /128, set CPOL high
	REG_PWM_CPRD1 = 0x3345;	//20ms
	REG_PWM_CDTY1 = 0x3d8;	//1.5ms
	REG_PWM_ENA |= (1 << 1);
}

void update_duty_cycle(int pos) {
	float new_dt = ((float) (-pos + 100)) / 200.0;
	if (new_dt < 0.0) {
		new_dt = 0.0;
	}
	if (new_dt > 1.0) {
		new_dt = 1.0;
	}
	int dt = (1.0 + new_dt) * ((float) 0x3d8 / 1.5);
	REG_PWM_CDTYUPD1 = dt;
	
}

void adc_init() {
	PMC->PMC_PCER1 |= (1 << 5); //ADC id = 37
	
	REG_ADC_MR |= (1 << 7) | (5 << 8);// | (2 << 24);	//Set Freerun mode and set prescaler to 50
	REG_ADC_CHER = (1 << 11); //Enable A11 as input for adc
	//REG_ADC_CHER = 0xffff;
	REG_ADC_CR |= (1 << 1); //start adc conversion
	
	//Tror disse bare er å lese
	//REG_ADC_CDR[n]
	//REG_ADC_LCDR
	
}

uint16_t get_adc_data() {
	return ADC->ADC_CDR[11];
	//return ADC->ADC_LCDR;
}