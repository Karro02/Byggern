#pragma once

#include "xmem.h"

#define COMMAND_OFFSET 0x0000
#define DATA_OFFSET 0x0200
#define false 0
#define true 1


void OLED_init();
void OLED_clear();
void OLED_write_data(char c);
void OLED_print(char* word);
void OLED_go_to_line(int line);
void OLED_clear_line(int line);
void OLED_home(signedPos offset);
void OLED_sub_menu();