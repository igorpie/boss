#include "ProjectMain.h"
#include "debug.h"
#include "HelperLib.h"
#include "jr_flash_103.h"
#include "jr_usart_103_hal.h"

DevStruct dev;
KeyStruct keys[] = {
		{ SW_1_Pin , GPIOA , 0 , 0 , 0 , 0   },		// A1
		{ SW_2_Pin , GPIOB , 0 , 0 , 0 , 0   },		// B2
};

uint16_t Adc1ConvertedValue[ADC_NUM_CHANNELS];
uint16_t Adc1Values[ADC_NUM_CHANNELS][kNumMeasure];

// data saving to flash
PotStruct pots[ADC_NUM_CHANNELS];								// светодиодные входы обрабатываются как потенциометры

// unsigned int PresetNumber = 7;	// костыль: номер пресета сохраняется в неиспользуемой ячейке потенциометра, так проще писать во флэш.
#define PresetNumber pots[0].trig[3]

void ProjectMain(void){
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;							// пуск таймера
	TIM4->CR1 |= TIM_CR1_CEN;
	USART_init();
	PotsInit();

	TestFlash();												// uncomment for test

	while(1){
		TriggersResetAll();
		pause(1);

		// опрос и обработка кнопок
		Key(&keys[0]);
		Key(&keys[1]);
		if (keys[0].flag_release_short == kSet) {
			keys[0].flag_release_short = kReset;
			(unsigned int)PresetNumber--;
			PresetNumber %= 10;
			debug2("Preset " , PresetNumber);
			// reload preset
		}
		if (keys[1].flag_release_short == kSet) {
			keys[1].flag_release_short = kReset;
			PresetNumber++;
			PresetNumber %= 10;
			debug2("Preset " , PresetNumber);
			// reload preset
		}
		if (keys[1].flag_hold_longer == kSet) {
			keys[1].flag_hold_longer = kReset;
			FlashSave((char *) pots, 96*(ADC_NUM_CHANNELS-2));	// состояние входов лампочек не сохраняется
			// save to flash
			debug("Save to flash OK");
		}

		// опрос и обработка потенциометров. Миди прикидывается потенциометрами для СС и кнопками для РС
		ScanPotsShadow();
		for (int i = 0 ; i < ADC_NUM_CHANNELS ; i++) {
			if (pots[i].trig[0] == kSet)
				debugAdc();
		}
	}
};


void UsbReceivedMidiCC(int byte1 , int byte2 , int byte3){
	debug2("USB MIDI CC: " , byte1);
	debug2("USB MIDI Controller: " , byte2);
	debug2("USB MIDI Value: " , byte3);
};

void UsbReceivedMidiPC(int byte1 , int byte2){
	debug2("USB MIDI PC: " , byte1);
	debug2("USB MIDI Value: " , byte2);
};


void TestFlash(void) {

	debug("Flash tests");
	debugAdc();

	PresetNumber = 0x12345678;
	for (int i = 0; i < 3000 ; i++ ) {
		TriggersResetAll();
		pause(1);
		ScanPotsShadow();
	}
	FlashSave((char *) pots, 96 * (ADC_NUM_CHANNELS-2));
	debug("Save to flash test OK");

	CLEARS((char *) pots, 96 * (ADC_NUM_CHANNELS-2));			// clean RAM area

	FlashLoad((char *) pots, 96 * (ADC_NUM_CHANNELS-2));
	debugAdc();
	debug("Flash load test OK");
}
