#pragma once
#include <stdio.h>

void servo_init();

void control_servo(int pos);

void adc_init();

uint16_t get_adc_data();

void motor_init();

void set_motor_direction(uint8_t dir);

void control_motor_speed(float speed); //between -100 and 100

void encoder_init();

uint32_t read_encoder();

void solenoid_init();

void activate_solenoid(int joy_btn);

uint32_t calibrate_encoder();

