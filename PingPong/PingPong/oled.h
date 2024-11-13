#pragma once

#include "xmem.h"
#include "mcp2515.h"

#define COMMAND_OFFSET 0x0000
#define DATA_OFFSET 0x0200
#define false 0
#define true 1

typedef enum {
	HOME = 0,
	MENU = 1,
	GAME = 2,
	GAME_OVER = 3
} SCREEN_GUI;

typedef char* screen_data[8];

void OLED_init();
void OLED_clear();
void OLED_write_data(char c, int invert);
void OLED_print(char* word);
void OLED_print_invert(char* word);
void OLED_print_screen(screen_data wanted_screen);
void OLED_go_to_line(int line);
void OLED_clear_line(int line);
//SCREEN_GUI OLED_home(signedPos offset);
SCREEN_GUI OLED_home(signedPos offset, int wanted_pos[]);
SCREEN_GUI OLED_sub_menu(signedPos offset);

int OLED_menu(signedPos offset, screen_data screen, int wanted_pos[], SCREEN_GUI gui);
void OLED_run(signedPos offset);