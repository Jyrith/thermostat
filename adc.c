
#include <avr/io.h>

#include "adc.h"
#include "defFile.h"


void initADC(void)
{
	// Default Voltage ref. is AREF, Internal Vref turned off
	// ADMUX = (1 << REFS0);

	// ADC prescaler 125 kHz, 16MHz/128 -> 125kHz, Enable ADC
	ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);
}

uint16_t readADC(uint8_t channel)
{
	uint8_t temp = ADMUX;	// Read current ADMUX settings
	temp &= ~0x0F;			// clear bits MUX0...MUX3

	// ADC0...ADC8
	temp |= channel;		// set new channnel
	ADMUX = temp;			// Write a new value to ADMUX

	ADCSRA |= (1 << ADSC);

	// wait until conversion is done.
	while (ADCSRA & (1 << ADSC))
		;
	int16_t adcValue = ADC;
	// sum += (ADCH << 8 );
	return adcValue;
}

uint16_t readADC_Average(uint8_t channel)
{
	uint16_t average = 0;
	int8_t i=0;
	for (i=0;i<(1<<AVERAGESIZE);i++)
		average += readADC(channel);
	average = average>>AVERAGESIZE;	//
	return average;
}


