
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>#include <stdio.h>

#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

#include "libraries/adc/adc.h"
#include "lcd.h"

#define BUTTON_PORT       PORTD
#define BUTTON_DDR        DDRD
#define BUTTON_PIN        PD3
#define BUTTON_INT        INT3
#define BUTTON_INT_MODE   (1 << ISC31)
#define BUTTON_READ_VAL   PIND & (1 << BUTTON_PIN)
#define BUTTON_SHORT      10 // *10 ms
#define BUTTON_LONG       150 // *10 ms

volatile unsigned long timer = 0;
double oxygen = 0;
int max_oxygen_value = 605;

volatile unsigned int button_holdtime = 0;
volatile int button_down = 0;

volatile enum {
	NO,
	SHORT,
	LONG
} button_pushed;

// 1 ms resolution timer and button debounce / hold
ISR(TIMER1_COMPA_vect)
{
	timer++;

	if (button_down && (timer % 10 == 0)) {
		// read button status
		if (BUTTON_READ_VAL) {
			// Button was released
			if (button_holdtime > BUTTON_SHORT) {
				// This was a short push, short push triggered
				button_pushed = SHORT;
			} else {
				// Really short push, enable interrupt again..
				EIMSK |= (1 << BUTTON_INT);
			}
			button_down = 0;
		} else {
			// Button is still held
			button_holdtime++;
			if (button_holdtime > BUTTON_LONG) {
				// Button was held really long, long push trigged
				button_pushed = LONG;
				button_down = 0;
			}
		}
	}
}

ISR(INT3_vect)
{
	button_down = 1;
	button_holdtime = 0;
	// Disable button interrupt
	EIMSK ^= (1 << BUTTON_INT);
}

// Set up a 1 ms timer1.
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

void button_init()
{
	// Button pin input
	BUTTON_DDR |= (1 << BUTTON_PIN);
	// Button pin pull up resistor
	BUTTON_PORT |= (1 << BUTTON_PIN);
	// Falling edge interrupt
	EICRA |= BUTTON_INT_MODE;
	// Enable external interrupt:
	EIMSK |= (1 << BUTTON_INT);
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
	button_init();

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
		/*if (button_pushed) {
			lcd_display_oxygen((button_pushed == LONG) ? 99 : 50);
			button_pushed = NO;
			EIMSK |= (1 << BUTTON_INT);
		}*/
	}
}
