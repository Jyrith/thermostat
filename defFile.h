
#ifndef DEFFILE_H
#define DEFFILE_H

/**
 *
 *	@file defFile.h
 * 	@code #include <tools.h> @endcode
 *
 *	@brief Helper functions.
 *
 *	- Bit-mask operation
 *	- Rotary decoder counter
 *	- States in main for-loop
 *
 *
 *	@author J.Karlbom
 */

#define BitSet(port,bit) ((port) |= (1<< (bit)))
#define BitClear(port,bit) ((port) &= ~(1<< (bit)))
#define BitCheck(port,bit) ((port) & (1<< (bit)))


volatile uint8_t encoderCounter;	//!< Counts rotary encoder rotations

//!< States in main
#define STATE_OFF 0x01		//!< States in main
#define STATE_SET 0x02
#define STATE_ADC 0x03

	//!< Decimal point control
#define DIGIT_OFF	0		//!< Don't use decimal point digit on display
#define DIGIT_ON	1		//!< Use decimal point digit on display


#define SSR_PORT	PORTB	//!< SSR control settings
#define SSR_DDR		DDRB
#define SSR_CTR_PIN	PB1		//!< SSR control pin
#define SSR_LED_PIN	PB5		//!< SSR control status LED pin

#endif //DEFFILE_H
