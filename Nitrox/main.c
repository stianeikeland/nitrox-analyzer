
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>#include <stdio.h>

#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

#include "lcd.h"

#define BUTTON_PORT       PORTD
#define BUTTON_DDR        DDRD
#define BUTTON_PIN        PD3
#define BUTTON_INT        INT3
#define BUTTON_INT_MODE   (1 << ISC31)
#define BUTTON_READ_VAL   PIND & (1 << BUTTON_PIN)
#define BUTTON_SHORT      10 // *10 ms
#define BUTTON_LONG       150 // *10 ms

#define POWER_ON_PIN      PC0
#define POWER_ON_PORT     PORTC
#define POWER_ON_DDR      DDRC

#define BACKLIGHT_DDR     DDRB
#define BACKLIGHT_PORT    PORTB
#define BACKLIGHT_PIN     PB6

#define MAX_IDLE_TIME     300 // in sec

volatile unsigned long timer = 0;
volatile unsigned long idle_time = 0;

volatile unsigned int button_holdtime = 0;
volatile int button_down = 0;

volatile double adc_filtered_result = 0;
double adc_air_level = 100;
double oxygen = 0;

volatile enum {
	NO,
	SHORT,
	LONG
} button_pushed;

enum {
	CALIBRATION,
	ANALYZING,
	ANALYZING_HOLD
} program_state = ANALYZING;

ISR(INT3_vect)
{
	button_down = 1;
	button_holdtime = 0;
	// Disable button interrupt
	EIMSK ^= (1 << BUTTON_INT);
}

ISR(ADC_vect)
{
	// Read out ADC result
	unsigned long result = ADCL;
	result += (ADCH << 8);
	adc_filtered_result = ((double)result * 0.001) + (adc_filtered_result * 0.999);
}

// 1 ms resolution timer and button debounce / hold
ISR(TIMER1_COMPA_vect)
{
	timer++;

	if (timer % 1000 == 0)
		idle_time++;

	if (button_down && (timer % 10 == 0))
		handle_button_release();
}

inline void handle_button_release()
{	
	if (BUTTON_READ_VAL) {
		
		// Button was released, enable interrupt again if it was shorter
		// than threshold for short push.
		if (button_holdtime > BUTTON_SHORT)
			button_pushed = SHORT;
		else
			EIMSK |= (1 << BUTTON_INT);
		
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

// Set up a 1 ms timer1.
static void timer1_init()
{
	TCCR1B |= (1 << WGM12);
	// Target - 1000 Hz at 8 Mhz (8000000 / 8) / 1000 - 1
	OCR1A = 999;
	// Start timer, prescaler 8
	TCCR1B |= (1 << CS11);
	// CTC interrupt:
	TIMSK1 |= (1 << OCIE1A);
}

static void button_init()
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

static void power_on()
{
	// Power control pin high.
	POWER_ON_DDR |= (1 << POWER_ON_PIN);
	POWER_ON_PORT |= (1 << POWER_ON_PIN);
}

static void backlight_init()
{
	// Turn on backlight
	BACKLIGHT_DDR |= (1 << BACKLIGHT_PIN);
	BACKLIGHT_PORT |= (1 << BACKLIGHT_PIN);
}

static void adc_init()
{
	// Slowest ADC Prescaler
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	// Set ADC reference to AVCC, and channel to 1
	ADMUX |= (1 << REFS0) | (1 << MUX0);
	// Free running mode, enable adc, enable interrupt, go!
	ADCSRA |= (1 << ADATE) | (1 << ADEN) | (1 << ADIE) | (1 << ADSC);
}

void enable_button_interrupt()
{
	EIMSK |= (1 << BUTTON_INT);
	button_pushed = NO;

	// Reset idle timer
	idle_time = 0;
}

double get_oxygen_level(double air_level, double adc_level)
{
	// 20.9% oxygen = adc_air_level
	double slope = air_level / 20.9;
	return (slope > 0) ? (adc_level / slope) : 0;
}


static void loop()
{	
	// Cut power if idle threshold reached
	if (idle_time > MAX_IDLE_TIME)
		POWER_ON_PORT &= ~(1 << POWER_ON_PIN);

	switch (program_state) {

		case ANALYZING:
			oxygen = get_oxygen_level(adc_air_level, adc_filtered_result);   /* FALLTHROUGH */

		case ANALYZING_HOLD:
			lcd_display_oxygen(oxygen, (program_state == ANALYZING_HOLD));
		
			if (button_pushed == SHORT)
				program_state = (program_state == ANALYZING) ? ANALYZING_HOLD : ANALYZING;
			
			if (button_pushed == LONG)
				program_state = CALIBRATION;
			
			if (button_pushed)
				enable_button_interrupt();
			
			break;
		
		case CALIBRATION:
			lcd_display_calibration(adc_filtered_result);
		
			if (button_pushed == LONG) {
				program_state = ANALYZING;
				adc_air_level = adc_filtered_result;
			}
		
			if (button_pushed)
				enable_button_interrupt();

			break;
	}
}

int main(void)
{
	power_on();
	timer1_init();
	lcd_init();
	button_init();
	adc_init();
	backlight_init();

	// Enable interrupts:
	sei();

	while (1)
		loop();
}
