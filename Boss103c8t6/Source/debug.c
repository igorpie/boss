/*
 * debug.c
 *
 */

#include "debug.h"
#include "ProjectMain.h"
#include "HelperLib.h"
#include "jr_flash_103.h"
#include "jr_usart_103_hal.h"

void debugAdc(void){
	int i;

#if 0
	crlf();
	for (i=0 ; i < ADC_NUM_CHANNELS ; i++) {
		echo_u33("   Adc" , i);
		echo_u33(" = " , Adc1ConvertedValue[i] >> 4);
	}
#endif
	crlf();
	for (i=0 ; i < ADC_NUM_CHANNELS ; i++) {
		echo_u33(" Adc" , i);
		echo_u33("=" , pots[i].val_int[0]);
	}
	crlf();
}

void debugState(void){
	//debug2("Preset " , pots[0].trig[3]);
	debug2("Preset " , presetNumber);
	debug2("clock " , dev.clock);
}


void TestFlash(void) {
	debug("Flash tests");
	debugAdc();

	presetNumber = 0x12345678;
	for (int i = 0; i < 3000 ; i++ ) {
		TriggersResetAll();
		//pause(1);
		ScanPotsShadow();
	}
	FlashSave();
	debug("Save to flash test OK");
	CLEARS((char *) presets, kSizePreset * PRESETS_NUM);			// clean RAM area
	FlashLoad();
	debugAdc();
	debug("Flash load test OK");
}
