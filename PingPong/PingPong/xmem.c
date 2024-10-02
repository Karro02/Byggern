#include "xmem.h"

#include <avr/io.h>


void xmem_init() {
	//SRAM
	MCUCR |= 1 << SRE; // enable external memory interface
	SFIOR &= ~(1 << XMM1 | 1 << XMM0); //setting XMM1 and XMM0 to 0 Worked with XMM2 instead of XMM0
	SFIOR |= 1 << XMM2; // masking out the bits that JTAG uses Set other ones to 0
	
	// ADC
	DDRD = 1 << DDD4; // set PD4 as PWM clock signal
	TCCR3A |=  1 << WGM30 | 0 << WGM31 | 0 << COM3A0 | 1 << COM3A1;
	TCCR3B |= 1 << WGM32 | 0 << ICES3 | 1 << CS30;
	
	
	//Buttons
	DDRB |= 0 << DDB2 | 0 << DDB1 | 0 << DDB0; 
	PORTB |= 1 << PB1;
	//EMCUCR = 0 << SRW10 | 0 << SRW11;
	

}

void xmem_write ( uint8_t data , uint16_t addr ) {
	volatile char * ext_mem = ( char *) BASE_ADDRESS ;
	ext_mem [ addr ]= data ;
}

uint8_t xmem_read ( uint16_t addr ) {
	volatile char * ext_mem = ( char *) BASE_ADDRESS ;
	uint8_t ret_val = ext_mem [ addr ];
	return ret_val ;
}
	
	

joystickAndSliderPos get_board_data() {
	xmem_write(1, ADC_OFFSET);
	joystickAndSliderPos data;
	data.L_slider = (int16_t) xmem_read(ADC_OFFSET);
	data.Y_joystick =  (int16_t) xmem_read(ADC_OFFSET);
	data.X_joystick = (int16_t) xmem_read(ADC_OFFSET);
	data.R_slider = (int16_t) xmem_read(ADC_OFFSET);
	return data;
}

signedPos get_stick_offset() {
	signedPos Offset;
	joystickAndSliderPos data = get_board_data();
	Offset.X = (int16_t) data.X_joystick - 128;
	Offset.Y = (int16_t) data.Y_joystick - 128;
	return Offset;
}


signedPos get_percent_pos(joystickAndSliderPos pos, signedPos offset) {
	signedPos Percent;
	if (pos.X_joystick > 128 + offset.X) {
		Percent.X = (int16_t) (((double) (pos.X_joystick - offset.X - 128)) / (128 - offset.X) * 100.0);
		} else {
		Percent.X = (int16_t) (((double) (pos.X_joystick - offset.X - 128)) / (128 + offset.X) * 100.0);
	}
	
	if (pos.Y_joystick > 128 + offset.Y) {
		Percent.Y = (int16_t) (((double) (pos.Y_joystick - offset.Y - 128)) / (128 - offset.Y) * 100.0);
		} else {
		Percent.Y = (int16_t) (((double) (pos.Y_joystick - offset.Y - 128)) / (128 + offset.Y) * 100.0);
	}
	
	return Percent;
}

sliderPos get_slider_pos(joystickAndSliderPos pos) {  //returns in percent
	sliderPos S;
	S.R = (int16_t) (((double) (pos.R_slider)) / 255 * 100.0);
	S.L = (int16_t) (((double) (pos.L_slider)) / 255 * 100.0);
	return S;
}

JOYSTICKPOS get_discrete_direction(signedPos offset) {
	signedPos percent = get_percent_pos(get_board_data(), offset);
	if (percent.X > -80 && percent.X < 80 && percent.Y > -80 && percent.Y < 80) {  //change value for sensitivity
		return NEUTRAL;
	}
	if (abs(percent.X) > abs(percent.Y))  {
		if (percent.X < 0) {
			return LEFT;
		} else {
			return RIGHT;
		}
	} else {
		if (percent.Y < 0) {
			return DOWN;
		} else {
			return UP;
		}
	}
}


buttonData get_button_data() {
	int data = PINB;
	buttonData button_data;
	button_data.joy_button = !((PINB >> 1) & 0b1);  //At pb2
	button_data.l_button = (PINB >> 0) & 0b1;	 //At pb1
	button_data.r_button = (PINB >> 2) & 0b1;	 //At pb0
	return button_data;
}