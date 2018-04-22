

#ifndef DISPLAY595_H
#define DISPLAY595_H

/**
 *	@file display595.h
 *	@brief Driver for 595 type of 7-segment display.
 *
 *	@author J.Karlbom
 */


#define CTR_PORT	PORTB	//!< Port where display is connected
#define CTR_DDR		DDRB	//!< Ports DDR register
#define SCLK 		PB4		//!< Shift register clock, data clocked in from DIO-pin.
#define RCLK 		PB3		//!< Storage register clock, data is transferred to output pins.
#define DIO 		PB2		//!< Data in pin (595 DS-pin 14).


/**
 *	@brief Initialize control pins
 *
 *	@return	void
 */
void initPins (void);


/**
 *	@brief Send one number to segment with RowClock
 *
 *	@param number A number to display.
 *				Value can be 0...9.
 *	 			And with decimal point 10...19. and 20 = '-' char
 *				All LEDs off with 21
 *
 *	@param segment	Segment index where to show a number. Left-most segments index is 1.
 *
 *	Example: Number '9' to first segment
 *		displayNumber(9,1);
 *
 */
void displayNumber(uint8_t number, uint8_t segment);


/**
 *	@brief Clear display
 *
 */
void displayClear();

#endif	//DISPLAY595_H
