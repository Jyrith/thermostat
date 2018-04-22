
#ifndef DEBOUNCEKEY_H
#define DEBOUNCEKEY_H

/**
 *	@file debounceKey.h
 *	@brief Debouncing Keys and rotary encoder.
 *
 *	The state of the keys is sampled four times using a timer interrupt.
 *	- Encoder interface
 *	- Read rotary encoder
 *	
 *	ATMega328:
 *	- TIMER1_OVF_vect overflow interrupt
 *
 *	Debounce interface
 *	- When a key is pressed longer than four times the sampling period,
 *	- the corresponding bit in keyState is set.
 *
 *	@code
 *	// single press:
 *	if( getKeyPress( KEY1 ))	// Key 1:
 *	@endcode
 *
 *	Based on code of Peter Dannegger & Peter Fleury
 *
 */


#include <avr/io.h>

#define KEY_PIN		PIND
#define KEY_PORT	PORTD
#define KEY_DDR		DDRD
#define KEY1 (1<<PD4)					//!< Rotary encoder press button
#define KEY2 (1<<PD5)					//!< On/Off switch
#define KEY_MASK (KEY1 | KEY2)

#define KEYAB_PIN	PIND
#define KEYAB_DDR	DDRD
#define KEYA (1<<PD2)
#define KEYB (1<<PD3)
#define PHASE_A     (KEYAB_PIN & KEYA)	//!< Rotary encoder
#define PHASE_B     (KEYAB_PIN & KEYB)	//!< Rotary encoder

//!< For debouncing keys
static volatile uint8_t keyPress;	//!< Key press detect
static volatile uint8_t keyState;	//!< Debounced and inverted key state:

//!< For rotary encoder
volatile int8_t encoderDelta;		//!< -128 ... 127


/**
 *  @brief Initialize pins to input & enable pullups and keyState variable.
 *  @param none
 *  @return none
 *
 */
void initDebounce( void );


/**
 *  @brief Return non-zero if a key matching mask is pressed.
 *  @param keyMask
 *  @return Return non-zero if a key matching mask is pressed.
 *
 */

uint8_t getKeyPress( uint8_t keyMask );


/**
 *  @brief Reads single step encoder. Atomic op
 *  @param none
 *  @return Counts since last call

 */
int8_t encodeRead( void );


/**
 *  @brief Reads two step encoder. Atomic op
 *  @param none
 *  @return Counts since last call
 *
 */
int8_t encodeTwoStepRead( void );


/**
 *  @brief Reads four step encoder. Atomic op
 *  @param none
 *  @return Counts since last call
 *
 */
int8_t encodeFourStepRead( void );

#endif	// DEBOUNCEKEY_H
