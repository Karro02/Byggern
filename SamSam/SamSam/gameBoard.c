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

void control_servo(int pos) {
	if ((pos <= 2) && (pos >= -2)) {
		pos = 0;
	}
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

void motor_init() {
	//Enabler PWM som peripheral
	PMC->PMC_PCER1 |= (1 << 4); //PIDn = 36
	
	//Fjerner PIO kontroll på PB13
	//Bruker PWM out på PB12 som er satt ved alternate function b
	PIOB->PIO_PDR |= (1 << 12); //PIO_PDR_Pn;
	
	//Velger B funksjon(PWMH0)
	PIOB->PIO_ABSR |= (1 << 12); //PIO_ABSR_Pn;
	
	//Sette opp PWM signalet MCK = 84 MHz
	//PWM->PWM_CLK / REG_PWM_CLK;
	REG_PWM_CMR0 |= (0b0111) | (1 << 9); //Prescaler = /128, set CPOL high
	REG_PWM_CPRD0 = 0x3345;	//20ms
	REG_PWM_CDTY0 = 0; //0x3d8;	//1.5ms
	REG_PWM_ENA |= (1 << 0);
	
	//Konfigurere direction pin
	REG_PMC_PCER0 |= (1 << ID_PIOC); //Enable PIOC
	REG_PIOC_PER |= (1 << 23); //Enable PC23
	REG_PIOC_OER |= (1 << 23);
}

void set_motor_direction(uint8_t dir) {
	if (dir) {
		REG_PIOC_SODR |= (1 << 23);
	} else {
		REG_PIOC_CODR |= (1 << 23);
	}
}

void control_motor_speed(float speed) {
	
	//float dt = ((float) (-pos + 100.0)) / 200.0 * (float) 0x3345;
	//float new_dt = ((float) (-pos + 100)) / 200.0;
	set_motor_direction(speed > 0.0);
	if (speed < 0.0) {
		speed = -speed;
	}
	//printf("s: %f\n", speed);
	float new_dt = (float) speed / 100.0;
	if (new_dt < 0.0) {
		new_dt = 0.0;
	}
	if (new_dt >= 1.0) {
		new_dt = 0.99;
	}
	//printf("d: %f\n", new_dt);
	//int dt = (1.0 + new_dt) * ((float) 0x3d8 / 1.5);
	int dt = new_dt * ((float) 0x3345);
	REG_PWM_CDTYUPD0 = (int) dt;
	
	
}

void encoder_init() {
	// Enable TC2
	//PMC->PMC_PCER0 |= (1 << 29); 
	
	//PMC->PMC_PCER0 |= (1 << ID_PIOC);
	
	//Enable TC2 channel 0
	PMC->PMC_PCER1 |= (1 << (ID_TC6 - 32));
	
	// Select B peripheral on PC25 (encoder input)
	//PIOC->PIO_OER |= (1 << 25);
	PIOC->PIO_PDR |= (1 << 25);
	PIOC->PIO_ABSR |= (1 << 25);
	// Select B peripheral on PC26 (encoder input)
	//PIOC->PIO_OER |= (1 << 26);
	PIOC->PIO_PDR |= (1 << 26);
	PIOC->PIO_ABSR |= (1 << 26);
	
	// measure position and enable quad encoder
	TC2->TC_BMR |= TC_BMR_POSEN | TC_BMR_QDEN | TC_BMR_EDGPHA;
	TC2->TC_CHANNEL[0].TC_CCR |= TC_CCR_CLKEN | TC_CCR_SWTRG;
	TC2->TC_CHANNEL[0].TC_CMR |= TC_CMR_TCCLKS_XC0; //| TC_CMR_ETRGEDG_RISING | TC_CMR_ABETRG;
}

uint32_t read_encoder() {
	return TC2->TC_CHANNEL[0].TC_CV;
}

void solenoid_init() {
	PMC->PMC_PCER0 |= (1 << ID_PIOC);
	PIOC->PIO_PER |= (1 << 28);
	PIOC->PIO_OER |= (1 << 28);
	
	//Høy er ikke aktiv.
	PIOC->PIO_SODR |= (1 << 28);
}


void activate_solenoid(int joy_btn) {
	if(joy_btn) {
	PIOC->PIO_CODR |= (1 << 28);
	} else {
	PIOC->PIO_SODR |= (1 << 28);
	}
}


uint32_t calibrate_encoder() {
	uint32_t old_enc = read_encoder();
	uint32_t new_enc = old_enc;
	control_motor_speed(-50);
	long sleep = 0;
	while (abs(new_enc) >= abs(old_enc)) {
		old_enc = new_enc;
		new_enc = read_encoder();
	}
	uint32_t min_enc = old_enc;
	control_motor_speed(50);
	old_enc = read_encoder();
	new_enc = old_enc;
	while (abs(new_enc) >= abs(old_enc)) {
		old_enc = new_enc;
		new_enc = read_encoder();
	}
	uint32_t max_enc = old_enc;
	control_motor_speed(0);
	return (abs(max_enc - min_enc))/2;
}

