
#ifndef ADC_H
#define ADC_H


/**
 *	
 	@file adc.h
 	@code #include <adc.h> @endcode

	@brief	AD-Converter functions for ATMega168 (ATMega328).

		- 7 ADC channels.

	@author J.Karlbom
 */


/**
 *	
	@brief Definitions for average value
		- AVERAGE_SIZE
 */
#define AVERAGESIZE 3


/**
 *	
	@brief	Initilize ADC conversion.
	@return	void
 */
void initADC(void);

/**
 *	
	 *	@brief	Start ADC conversion and read a result.

	 Starts ADC conversion and waits until the conversion is done.

	 *	@param	channel Singled ended ACD channel to read (0...8 or on-chip temperature sensor)
	 *	@return	ADC result 0...1024

 */
uint16_t readADC(uint8_t channel);

/**
 *	
 *	@brief	Start ADC conversion and read a result.
 *	@param	channel Singled ended ACD channel to read (0...8 or on-chip temperature sensor)
 *	@return	ADC result 0...1024
 *
 *	Starts ADC conversion and waits until the conversion is done.
 *
 */
uint16_t readADC_Average(uint8_t channel);


#endif //ADC_H
