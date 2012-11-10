
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>#include <stdio.h>

#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

#include "libraries/adc/adc.h"
#include "lcd.h"

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

int main(void)
{
	timer1_init();
	lcd_init();

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
			lcd_display_oxygen(oxygen);
			schedule_draw = timer + 150;
		}							
	}
}
