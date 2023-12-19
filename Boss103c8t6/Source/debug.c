/*
 * debug.c
 *
 */

#include "debug.h"
#include "ProjectMain.h"
#include "HelperLib.h"
#include "jr_usart_103_hal.h"

void debugAdc(void){

	int i;
	crlf();

	for (i=0 ; i < ADC_NUM_CHANNELS ; i++) {
		echo_u33("   Adc" , i);
		//echo_float("=" , pots[i].value);
		echo_u33(" = " , Adc1ConvertedValue[i] >> 4);
	}
	crlf();
	for (i=0 ; i < ADC_NUM_CHANNELS ; i++) {
		echo_u33(" Adc" , i);
		echo_float("=" , pots[i].f2);
	}
	crlf();

}

