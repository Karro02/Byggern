#include "oled.h"
#include "fonts.h"

void OLED_init() {
	//hentet fra datablad
	xmem_write(0xae, COMMAND_OFFSET); // display off
	xmem_write(0xa1, COMMAND_OFFSET); //segment remap
	xmem_write(0xda, COMMAND_OFFSET); //common pads hardware: alternative
	xmem_write(0x12, COMMAND_OFFSET);
	xmem_write(0xc8, COMMAND_OFFSET); //common output scan direction:com63~com0
	xmem_write(0xa8, COMMAND_OFFSET); //multiplex ration mode:63
	xmem_write(0x3f, COMMAND_OFFSET);
	xmem_write(0xd5, COMMAND_OFFSET); //display divide ratio/osc. freq. mode
	xmem_write(0x80, COMMAND_OFFSET);
	xmem_write(0x81, COMMAND_OFFSET); //contrast control
	xmem_write(0x50, COMMAND_OFFSET);
	xmem_write(0xd9, COMMAND_OFFSET); //set pre-charge period
	xmem_write(0x21, COMMAND_OFFSET);
	xmem_write(0x20, COMMAND_OFFSET); //Set Memory Addressing Mode
	xmem_write(0x02, COMMAND_OFFSET);
	xmem_write(0xdb, COMMAND_OFFSET); //VCOM deselect level mode
	xmem_write(0x30, COMMAND_OFFSET);
	xmem_write(0xad, COMMAND_OFFSET); //master configuration
	xmem_write(0x00, COMMAND_OFFSET);
	xmem_write(0xa4, COMMAND_OFFSET); //out follows RAM content
	xmem_write(0xa6, COMMAND_OFFSET); //set normal display
	xmem_write(0xaf, COMMAND_OFFSET); // display on
	
	OLED_clear();
}

void OLED_clear() {
	
	for (int i = 0; i < 8; i++) {
		xmem_write(0xb0 + i, COMMAND_OFFSET);
		xmem_write(0x00, COMMAND_OFFSET);
		xmem_write(0x10, COMMAND_OFFSET);
		for (int j = 0; j < 128; j++) {
			xmem_write(0x00, DATA_OFFSET);
		}
	}
	
}

void OLED_write_data(char c, int invert) {
	int index = ((int) c) - 32;
	for (int i = 0; i < 8; i++) {
		if (invert) {
			xmem_write(~pgm_read_byte(&(font8[index][i])), DATA_OFFSET);
		} else {
			xmem_write(pgm_read_byte(&(font8[index][i])), DATA_OFFSET);
		}
		
	}
}

void OLED_print(char* word) {
	
	while ( *word != '\0') {
		OLED_write_data(*word, false);
		word++;
	} 
}

void OLED_print_invert(char* word) {
	while ( *word != '\0') {
		OLED_write_data(*word, true);
		word++;
	}
}

void OLED_print_screen(screen_data wanted_screen) {
	for (int i = 0; i < 8; i++) {
		OLED_go_to_line(i);
		OLED_print(wanted_screen[i]);
	}
}

void OLED_go_to_line(int line)
{
	unsigned char lines[] = {0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7};
	xmem_write(lines[line], COMMAND_OFFSET);
}

void OLED_clear_line(int line)
{
	OLED_go_to_line(line);
	OLED_print("                ");
}


int OLED_menu(signedPos offset, screen_data screen, int wanted_pos[], SCREEN_GUI gui) {

	OLED_print_screen(screen);
	int selected = wanted_pos[0];
	int should_move = false;
	int sleep = 0;
	
	int wanted_index = 0;
	
	if (gui == GAME)
	{	
		mcp2515_bit_modify(MCP_CANINTF, (1 << 0), (0 << 0));
		while(1)
		{	
			if (sleep > 10000) {
				if (mcp2515_read(MCP_CANINTF) & (1 << 0)) {
					printf("dod\n");
					mcp2515_bit_modify(MCP_CANINTF, (1 << 0), (0 << 0));
					return 6;
				}
				signedPos Joystick = get_percent_pos(get_board_data(), offset);
				int joy_button = get_button_data().joy_button;
				CAN_message msg = {3, 3, {(char) Joystick.X, (char) Joystick.Y, (char) joy_button}};
				mcp2515_load_mult_TX(TX0, msg);
				mcp2515_request_to_send(TX0);
				printf("X: %d Y: %d\n", Joystick.X, Joystick.Y);
				
				sleep = 0;
				}
			sleep++;
		}
		
	}
	
	while(1)
	{
		if (sleep > 10000) {
			JOYSTICKPOS P = get_discrete_direction(offset);
			if (should_move) {
				if (P == UP) {
					OLED_print(screen[selected]);
					if (wanted_index > 0) {
						wanted_index--;
						selected = wanted_pos[wanted_index];
					}
					else {
						wanted_index = sizeof(wanted_pos)/sizeof(wanted_pos[0]); //- 1;
						selected = wanted_pos[wanted_index];
					}
				}
				else if (P == DOWN) {
					OLED_print(screen[selected]);
					if (wanted_index < sizeof(wanted_pos)/sizeof(wanted_pos[0])) {
						wanted_index++;
						selected = wanted_pos[wanted_index];
					}
					else {
						wanted_index = 0;
						selected = wanted_pos[wanted_index];
					}
				}
				OLED_go_to_line(selected);
				OLED_print_invert(screen[selected]);
				should_move = false;
			}
			if (P == NEUTRAL) {
				should_move = true;
				buttonData buttons = get_button_data();

				if (buttons.joy_button){
					while(buttons.joy_button) {buttons = get_button_data();}
					for (int i = 0; i < sizeof(wanted_pos); i++) {
						if (selected == wanted_pos[i]) {
							return selected;
						}
					}
				}
			}
			
			
			sleep = 0;
		}
		sleep++;
	}
}

void OLED_set_brightness(int lvl)
{
	unsigned char lvls[] = {0x1A, 0x34, 0x4E, 0x68, 0x82, 0x9C, 0xB6, 0xD0, 0xEA, 0xFF};
	xmem_write(0x81, COMMAND_OFFSET);
	xmem_write(lvls[lvl], COMMAND_OFFSET);
}



void OLED_run(signedPos offset) {
	OLED_clear();
	SCREEN_GUI gui = HOME;
	screen_data home;
	home[0] = "----------------";
	home[1] = "                ";
	home[2] = "  welcome home  ";
	home[3] = "                ";
	home[4] = "      menu      ";
	home[5] = "                ";
	home[6] = "                ";
	home[7] = "----------------";
			
	int wanted_pos_home[] = {4, 4};
	screen_data menu;
	menu[0] = "                ";
	menu[1] = "                ";
	menu[2] = "   Start Game   ";
	menu[3] = "                ";
	menu[4] = "  Back to Home  ";
	menu[5] = "                ";
	menu[6] = "                ";
	menu[7] = "                ";
	
	int wanted_pos_menu[] = {2, 4};
		
	screen_data game;
	game[0] = "                ";
	game[1] = "                ";
	game[2] = "      Game      ";
	game[3] = "     Running    ";
	game[4] = "                ";
	game[5] = "                ";
	game[6] = "                ";
	game[7] = "                ";
	
	screen_data game_over;
	
	
	int wanted_pos_game[] = {6, 6};
	while (1) {
		
		if (gui == HOME)
		{	
			
			
			int pos = OLED_menu(offset, home, wanted_pos_home, gui);
			printf("%d", pos);
			if(pos == 4) {
				gui = MENU;
			}		
		}
		
		printf("%d", gui);
		
		if (gui == MENU)
		{	
			
			
			int pos = OLED_menu(offset, menu, wanted_pos_menu, gui);
			printf("%d", pos);
			if(pos == 4) {
				gui = HOME;
			}
			if(pos == 2)
			{
				gui = GAME;
			}
			
		}
		
		if (gui == GAME)
		{
			
			
			int pos = OLED_menu(offset, game, wanted_pos_game, gui);
			printf("%d", pos);
			if(pos == 6) {
				gui = GAME_OVER;
			}
			
		}
		if (gui == GAME_OVER)
		{
			game_over[0] = "                ";
			game_over[1] = "       :(       ";
			game_over[2] = "    Game Over   ";
			game_over[3] = "     Score:     ";
			game_over[5] = "                ";
			game_over[6] = "      Exit      ";
			game_over[7] = "                ";
			uint8_t score = mcp2515_read_RX(RX0);
			
			if (score < 10)
			{
				sprintf(game_over[4], "       -%d-      ", score);
			}
			else if (score < 100)
			{
				sprintf(game_over[4], "       -%d-     ", score);
			}
			else if (score < 1000)
			{
				sprintf(game_over[4], "      -%d-     ", score);
			}
			
			int pos = OLED_menu(offset, game_over, wanted_pos_game, gui);
			printf("%d", pos);
			if(pos == 6) {
				gui = MENU;
			}
			
		}
	}
}