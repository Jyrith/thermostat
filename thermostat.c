
/**
 *	@file thermostat.c
 *	@defgroup jyrith_main Main code
 *
 *	@brief	Water heater controller and temperature indicator.
 *
 *		- ATmega328p module, (Arduino Pro mini, 5V, 16MHz).
 *		- 595 based 7-segment display (4-bit LED Digital Tube Module).
 *		- Rotary encoder with switch (example Bourns PEC11).
 *		- SSR, 230VAC with 5VDC control input.
 *		- LM50 or similar temperature sensor, 500mV output at 0°C, 10mV/°C
 *			- Resolution used here is 1mV, 1mV <=> 0.1°C
 *		- 1,6V Voltage reference for ADC. 
 *			- Then max temperature to measure is 110°C (1600mV-500mV)/10mV/°C:
 *		- Or use a red LED as a voltage reference,
 *			- and drive it with very low current (10k ohm resistor and 5V supply)
 *			- And calculate a new value for mV_256 variable
 *			- based a voltage measured across the LED (voltage reference).
 *
 *
 *	@author J.Karlbom
 */

/**@{*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#include "defFile.h"
#include "display595.h"
#include "adc.h"
#include "debounceKey.h"

/**
 *	@brief	Updates 7-segment display
 *
 *	@param		int16_t value
 *				value to show on 7-seg display
 *
 *	@param		int8_t showDigit
 *				T/F value to select if decimal point is show for this value
 *
 */
static void updateDisplay(int16_t value, int8_t showDigit);

// Delay for 7-segment display, ms
static const uint8_t displayDelay = 5;

// internal eeprom, EEMEM address starting from location 0
uint8_t EEMEM ee_temperature;


/**
 *	Main
 */
int main (void)
	{
	// Shift constant, calculations are made with integer value
	// with an intent to avoid floating format.
	static const uint8_t bitShift = 8;

	// 500mV x 256, 500mV is output voltage of LM50 at 0°C
	// If output value is something else than 500mV at 0°C change this constant
	static const int32_t zeroCelsiusVoltage = 128000;

	// ADC_Reference Voltage (mV) / ADC_Resolution x 256,
	// 		Example 1600 / 1023 x 256 = 400
	// Recalculate this value if other ADC_Reference Voltage is used
	// Value 389 is for 1,556V LED reference
	static const int16_t mV_Step256 = 389;

	// 
	int32_t temperature256;
	int16_t temperature;


	// SSR control & indication pins as output
	SSR_DDR = (1<<SSR_CTR_PIN) | (1<<SSR_LED_PIN);

	// Turn SSR control OFF by default
	BitClear(SSR_PORT,SSR_CTR_PIN);
	BitClear(SSR_PORT,SSR_LED_PIN);


	uint8_t temperatureLimit = eeprom_read_byte (&ee_temperature);
	if (temperatureLimit == 0xFF)	// Default value of interal eeprom
		temperatureLimit = 80;		// Init value to start with, 80°C

	// Sleep mode for ATmega328 (or SLEEP_MODE_PWR_SAVE)
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	initPins();		// 595 display
	initADC();		// ADC
	initDebounce();	// Switch
	sei();



	volatile int8_t state = STATE_OFF;

	// Main loop
	for(;;)
		{
		uint8_t loopCounter;
		int16_t tempValue;

		// Read temperature value with ADC and
		// convert value to °C, 1mV <=> 0.1°C
		// Resolution used here is 1mV
		uint16_t adcValue = readADC_Average(3);
		temperature256 = ((int32_t) adcValue) * mV_Step256 - zeroCelsiusVoltage;
		temperature = (int16_t) (temperature256 >> bitShift);

		// Convert temperature limit value to mV value, (i.e. temperatureLimit *= 10)
		tempValue = (temperatureLimit << 3) + temperatureLimit + temperatureLimit;
		if (temperature >= tempValue)
			{
			// Turn SSR relay OFF
			BitClear(SSR_PORT,SSR_CTR_PIN);
			BitClear(SSR_PORT,SSR_LED_PIN);
			}

		switch (state)
			{
			case STATE_OFF:
				// Turn off displays LEDs
				displayClear();
				// Turn SSR relay OFF
				BitClear(SSR_PORT,SSR_CTR_PIN);
				BitClear(SSR_PORT,SSR_LED_PIN);
				// Go to sleep
				// sleep_mode();
				sleep_enable(); sei(); sleep_cpu(); sleep_disable();
				_delay_ms (50);
				// Turn SSR relay ON
				BitSet(SSR_PORT,SSR_CTR_PIN);
				BitSet(SSR_PORT,SSR_LED_PIN);
				break;
			case STATE_ADC:
				// Update display a few times before reading a new value from ADC
				// otherwise small changes in an ADC value will flicker on display
				// This type of 7-segment dislay needs to update constantly...
				for(loopCounter=0;loopCounter<20;loopCounter++)
					updateDisplay(temperature,DIGIT_ON);
				break;
			case STATE_SET:
				// Read value from a four step encoder (Bourns PEC11)
				// Set value for water temperature
				temperatureLimit += encodeFourStepRead();
				if(temperatureLimit > 100)
					temperatureLimit = 100;
				if(temperatureLimit < 20)	// This is more for test purpose
					temperatureLimit = 20;
				//updateDisplay(temperatureLimit,DIGIT_OFF);
				for(loopCounter=0;loopCounter<20;loopCounter++)
					updateDisplay(temperatureLimit,DIGIT_OFF);
				break;
			}

		if(getKeyPress(KEY2))
			{
			if (state != STATE_OFF)
				state = STATE_OFF;
			else if (state == STATE_OFF)
				state = STATE_ADC;
			}

		if(getKeyPress(KEY1))
			{
			if (state == STATE_ADC)
				state = STATE_SET;
			else if (state == STATE_SET)
				{
				state = STATE_ADC;
				eeprom_update_byte(&ee_temperature, temperatureLimit);
				}
			}

		}	// Main loop
	}		// Main

/**
 *	updateDisplay
 *	Needs to call constantly for this type of display.
 *	Max. value to show is 999 (99.9)
 */

static void updateDisplay(int16_t value, int8_t showDigit)
	{
    uint8_t digits[4] = {21,21,21,21};	// All segments are off
	uint8_t singleNumber;
	uint8_t segmentIndex=3;

    if (value < 0)   // Now left-most segment of 7-segment display shows minus sign
        {
        // Convert to positive value
        value = ~value +1;
        digits[3]='-';
        }
     

	singleNumber=0;
    if(value>99)			// Skip leading zero skip it here is one
        {
        while(value >= 100)	// 100's
            {
            singleNumber++;
            value -= 100;
            }
        digits[2]=singleNumber;
        }

	singleNumber=0;
    while(value >= 10)			// 10's
        {
        singleNumber++;
        value -= 10;
        }
	if (showDigit)			// Decimal point for last digit
		singleNumber +=10;	// Add 10 to number to show a decimal point
    digits[1]=singleNumber;
    digits[0]=value;
	
	// Show number on 7-segment display
    for(segmentIndex=4;segmentIndex>0;segmentIndex--)
        {
		displayNumber(digits[segmentIndex-1],segmentIndex);
        _delay_ms (displayDelay);
        }
	}

/**@}*/

