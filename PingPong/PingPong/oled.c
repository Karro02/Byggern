#include "oled.h"
#include "fonts.h"

void OLED_init() {
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

void OLED_write_data(char c) {
	int index = ((int) c) - 32;
	for (int i = 0; i < 8; i++) {
		xmem_write(pgm_read_byte(&(font8[index][i])), DATA_OFFSET);
	}
}

void OLED_print(char* word) {
	
	while ( *word != '\0') {
		OLED_write_data(*word);
		word++;
	} 
}

void OLED_go_to_line(int line)
{
	unsigned char lines[] = {0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb0, 0xb1, 0xb2};
	xmem_write(lines[line], COMMAND_OFFSET);
}

void OLED_clear_line(int line)
{
	OLED_go_to_line(line);
	OLED_print("                ");
}

void OLED_sub_menu()
{
	for(int i=0; i < 8; i++)
	{
		OLED_clear_line(i);
	}
	OLED_go_to_line(2);
	OLED_print("   Start Game   ");
	OLED_go_to_line(4);
	OLED_print("  Back to Home  ");
}

void OLED_home(signedPos offset)
{
	OLED_go_to_line(0);
	OLED_print("----------------");
	OLED_go_to_line(2);
	OLED_print("  Welcome home  ");
	OLED_go_to_line(4);
	OLED_print("      Menu      ");
	OLED_go_to_line(7);
	OLED_print("----------------");
	int menu = false;
	int selected = false;
	while(!menu)
	{
		signedPos P = get_percent_pos(get_board_data(), offset);
		if(abs(P.Y) > 50)
		{
			if(selected) {selected = false;}
			else {selected = true;}
			
			
		}
		if(selected /*& <<joystickbutton pressed*/)
		{
			menu = true;
		}
	}
	
}

void OLED_set_brightness(int lvl)
{
	unsigned char lvls[] = {0x1A, 0x34, 0x4E, 0x68, 0x82, 0x9C, 0xB6, 0xD0, 0xEA, 0xFF};
	xmem_write(0x81, COMMAND_OFFSET);
	xmem_write(lvls[lvl], COMMAND_OFFSET);
}