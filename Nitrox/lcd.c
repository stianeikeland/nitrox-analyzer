
#include "lcd.h"

void lcd_init()
{
	glcd_init();
	glcd_set_contrast(10);
	glcd_clear();
}

void lcd_display_oxygen(double oxygen_level, int holding)
{
	char oxygen_string[8];
	
	oxygen_level = MAX(MIN(99.99, oxygen_level), 0.00);
	
	glcd_clear_buffer();
	glcd_tiny_set_font(Font5x7,5,7,32,127);		if (holding) {		glcd_draw_string_xy(5,2,"Value Held");
		glcd_draw_string_xy(1,37,"Push resume");
	} else {
		glcd_draw_string_xy(7,2,"Analyzing");
		glcd_draw_string_xy(9,37,"% Oxygen");
	}
	
	glcd_set_font(Liberation_Sans15x21_Numbers,15,21,46,57);
	sprintf(oxygen_string, "%05.2f", oxygen_level);
	glcd_draw_string_xy(0,13,oxygen_string);
	
	double bar_value = oxygen_level * 256.0 / 100.0;
	glcd_bar_graph_vertical(72,0,10,44, (int)bar_value);
	glcd_write();
}

void lcd_display_calibration(double value)
{
	char value_string[8];
	
	glcd_clear_buffer();
	glcd_tiny_set_font(Font5x7,5,7,32,127);	
	glcd_draw_string_xy(2,2,"Calibration");
	glcd_draw_string_xy(8,37,"ADC Value");
	
	glcd_set_font(Liberation_Sans15x21_Numbers,15,21,46,57);
	sprintf(value_string, "%05.1f", value);
	glcd_draw_string_xy(0,13, value_string);
	
	double bar_value = value * 256.0 / 1024.0;
	glcd_bar_graph_vertical(72,0,10,44, (int)bar_value);
	glcd_write();
}
