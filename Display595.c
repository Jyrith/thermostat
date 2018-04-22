
/**
 *
  @file Display595.c
  @brief Driver for 595 type of 7-segment display.

 */

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "display595.h"
#include "defFile.h"


/**
 * Values for 7 segment display
 * 
 * 0 1 2 3 4 5 6 7 8 9
 * 0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -
 */
const uint8_t LED_7[] PROGMEM = 
	{0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,
	0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xBF,0xFF};


/**
 * Shift one number to 595
 * Internal routine
 */
static void shiftData(uint8_t number);


void initPins()
{
	// Configure pins as outputs
	CTR_DDR |= (1 << (SCLK)) | (1 << (RCLK)) | (1 << (DIO));
}

void displayNumber(uint8_t number, uint8_t segment)
{
	shiftData(pgm_read_byte(&LED_7[number]));
	// Select a correct segment display 
	shiftData(1<<(segment-1));
	// Storage register clock toggle (data is transferred to output pins of 595)
	BitClear(CTR_PORT,RCLK);
    BitSet(CTR_PORT,RCLK);
}

void displayClear()
	{
	// All LEDs off ( LED_7[21] )
	displayNumber(21,1);
	displayNumber(21,2);
	displayNumber(21,3);
	displayNumber(21,4);
	}

static void shiftData(uint8_t number)
{
	int8_t i;
	for (i = 8; i >= 1; i--)
	{
		if (number & 0x80) {
			BitSet(CTR_PORT,DIO);
		}
		else {
			BitClear(CTR_PORT,DIO);
		}
	number <<= 1;
	// Shift clock toggle, state of DIO pin in read.
	BitClear(CTR_PORT,SCLK);
    BitSet(CTR_PORT,SCLK);
	}
}
