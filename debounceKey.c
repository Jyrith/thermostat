
/**
 *
  @file debounceKey.c
  @brief Debouncing Keys and rotary encoder.

 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "debounceKey.h"


/**
 *	
 *	PCINT2_vect Pin Change Interrupt
 *
 */
ISR (PCINT2_vect)		
	{
		;
	}


/**
 *	
 *	TIMER0_OVF_vect overflow interrupt
 *
 */
ISR (TIMER0_OVF_vect)		
{
	/*	Reading debouncing keys
	*/
	static uint8_t ct0= 0xFF;	// holds two bit counter for each key
	static uint8_t ct1= 0xFF;	// holds two bit counter for each key
	static int8_t last;
	uint8_t i;

	/*	Read current state of keys (active-low),
		clear corresponding bit in i when key has changed
	*/
	i = ~KEY_PIN;			// read keys (low active)
	i ^= keyState;			// key changed ?

	/*	ct0 and ct1 form a two bit counter for each key,  
		where ct0 holds LSB and ct1 holds MSB
		After a key is pressed longer than four times the
		sampling period, the corresponding bit in keyState is set
	*/
	ct0 = ~( ct0 & i );		// reset or count ct0
	ct1 = (ct0 ^ ct1) & i;	// reset or count ct1  
	i &= ct0 & ct1;			// count until roll over ?
	keyState ^= i;			// then toggle debounced state
  
	/*	To notify main program of pressed key, the correspondig bit
		in global variable keyPress is set.
		The main loop needs to clear this bit
	*/
	keyPress |= keyState & i;	// 0->1: key press detect
  
	
	/*	Reading rotary encoder
	*/
	int8_t newPhase, diff;
 
	newPhase = 0;
	if( PHASE_A )
		newPhase = 3;
	if( PHASE_B )
		newPhase ^= 1;			// convert gray to binary
	diff = last - newPhase;		// difference last - newPhase
	if( diff & 1 )				// bit 0 = value (1)
	{
		last = newPhase;				// store newPhase as next last
		encoderDelta += (diff & 2) - 1;	// bit 1 = direction (+/-)
	}	
}

/**
 *	
 *	Initialize 
 */
void initDebounce(void)
{
	
	//	Timer0 (8 bit), Normal mode,
	// Prescaler 64 16 MHz CLK => ~1ms (1,024 ms) (16e6/256/64 =~ 977 Hz)
	TCCR0B |= (1<<CS01) | (1<<CS00);
	// Prescaler 256 16 MHz CLK => ~4ms (4,096 ms) (16e6/256/256 =~ 244 Hz)
	// TCCR0B |= (1<<CS02);
	// enable overflow interrupt
	TIMSK0 |= (1<<TOIE0);				
	
	// KEY pin as input
	KEY_DDR &= ~(KEY_MASK);
	// Enable pullup on KEYs
	KEY_PORT |= KEY_MASK;
	keyState = ~KEY_PIN;			// no action on keypress during reset

	// Pin Change Interrupt Enable 2
	PCICR |= 1<<PCIE2;
	// Pin Change Mask Register 2
	// for KEY1 & KEY2
	PCMSK2 |= (1<<PCINT20)|(1<<PCINT21);
	
	// KEYAB pins as input, rotary encoder
	KEYAB_DDR &= ~(KEYA|KEYB);
	// Enable pullup on KEYs
	KEY_PORT |= (KEYA|KEYB);	
}

	
/**
 *	
 *	getKeyPress( uint8_t keyMask )
 *
 */

uint8_t getKeyPress( uint8_t keyMask )
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)	// read and clear atomic !
	{	
		keyMask &= keyPress;		// read key(s)
		keyPress ^= keyMask;		// clear key(s)
	}
	return keyMask;
}



/**
 *	
 *	encodeRead( void )
 *
 */
int8_t encodeRead( void )			// read single step encoders
{
	int8_t val;

	ATOMIC_BLOCK(ATOMIC_FORCEON)	// read and clear atomic
	{
		val = encoderDelta;
		encoderDelta = 0;
	}
	return val;						// counts since last call
}


/**
 *	
 *	encodeRead( void )
 *
 */
int8_t encodeTwoStepRead( void )	// read single step encoders
{
	int8_t val;

	ATOMIC_BLOCK(ATOMIC_FORCEON)	// read and clear atomic
	{
		val = encoderDelta;
		encoderDelta = val & 1;
	}
	return val>>1;					// counts since last call
}

/**
 *	
 *	encodeRead( void )
 *
 */
int8_t encodeFourStepRead( void )	// read single step encoders
{
	int8_t val;

	ATOMIC_BLOCK(ATOMIC_FORCEON)	// read and clear atomic
	{
		val = encoderDelta;
		encoderDelta = val & 3;
	}
	return val>>2;					// counts since last call
}

