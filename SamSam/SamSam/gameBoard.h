#pragma once
#include <stdio.h>

void servo_init();

void update_duty_cycle(int pos);

void adc_init();

uint16_t get_adc_data();