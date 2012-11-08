
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>#include <stdio.h>

#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

#include "libraries/glcd/glcd.h"
#include "libraries/glcd/fonts/font5x7.h"
#include "libraries/glcd/fonts/Liberation_Sans15x21_Numbers.h"
#include "libraries/adc/adc.h"

volatile unsigned long timer = 0;
double oxygen = 0;
int max_oxygen_value = 605;

// 1 ms resolution timer
ISR(TIMER1_COMPA_vect)
{
	timer++;
}

// Set up a 1 ms timer.
void timer1_init()
{
	TCCR1B |= (1 << WGM12);
	// Target - 1000 Hz at 8 Mhz (8000000 / 8) / 1000 - 1
	OCR1A   = 999;
	// Start timer, prescaler 8
	TCCR1B |= (1 << CS11);
	// CTC interrupt:
	TIMSK1 |= (1 << OCIE1A);
}

double sample_adc(int tries)
{
	long sample = 0;
	int i;
	for (i=0; i<tries; i++)
		sample += readADC_buzywait(1,7);
	return (double)sample / tries;
}

void read_oxygen() 
{
	double new_oxygen = sample_adc(10) * 100 / (double)max_oxygen_value;
	oxygen = new_oxygen * 0.10 + oxygen * 0.90;
} 

void draw_lcd(double oxygen_level) 
{
	char oxygen_string[8];
	
	glcd_clear_buffer();
	
	glcd_tiny_set_font(Font5x7,5,7,32,127);	glcd_draw_string_xy(7,2,"Analyzing");
	glcd_draw_string_xy(9,37,"% Oxygen");
	
	glcd_set_font(Liberation_Sans15x21_Numbers,15,21,46,57);
	sprintf(oxygen_string,"%05.2f", oxygen_level);
	glcd_draw_string_xy(0,13,oxygen_string);
	
	double bar_value = oxygen_level * 256.0 / 100.0;
	glcd_bar_graph_vertical(72,0,10,44, (int)bar_value);
	glcd_write();
}

int main(void)
{
	timer1_init();
	glcd_init();
	glcd_set_contrast(10);
	glcd_clear();
	
	// Enable interrupts:
	sei();
	
	double schedule_draw = 0;
	double schedule_read = 0;
	
	while (1) {
		if (timer >= schedule_read) {
			read_oxygen();
			schedule_read = timer + 10;
		}
		if (timer >= schedule_draw) {
			draw_lcd(oxygen);
			schedule_draw = timer + 150;
		}			
					
	}
}