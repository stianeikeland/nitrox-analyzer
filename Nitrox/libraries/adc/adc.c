/**
\file adc.c
\brief ADC driver, simplistic...

\author Bernt Hustad Hembre
**/
#include <avr/io.h>

/**
\brief measures and returns the ADC value for the selected channel 
\warning buzy waits for the conversion to complete
\arg	ch	The ADC channel to use.
\arg	speed The ADC speed. 0 is fastest, 7 is slowest (ATMega128)
\ret the ADC value (0-1024) or -1 if the chanel is invalid.
**/
int readADC_buzywait(char ch, char speed)
{
	if(speed > 7)
		speed = 7;
	if(ch>7)
		return -1;
	ADMUX = ch;
	ADCSRA = ((1<<ADEN)|(1<<ADSC)) + speed;
	while(ADCSRA& (1<<ADSC))
		;/* wait for analog to digital conversion to complete */
	return(ADCL + ((ADCH & 0x03)*256));
}

