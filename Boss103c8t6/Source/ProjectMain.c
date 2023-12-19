#include "ProjectMain.h"
#include "debug.h"
#include "HelperLib.h"
#include "jr_usart_103_hal.h"

DevStruct dev;
KeyStruct keys[] = {
		{ SW_1_Pin , GPIOA , 0 , 0 , 0 , 0   },		// A1
		{ SW_2_Pin , GPIOB , 0 , 0 , 0 , 0   },		// B2
};

uint16_t Adc1ConvertedValue[ADC_NUM_CHANNELS];
uint16_t Adc1Values[ADC_NUM_CHANNELS][kNumMeasure];
PotStruct pots[ADC_NUM_CHANNELS];


void ProjectMain(void){
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;							// пуск таймера
	TIM4->CR1 |= TIM_CR1_CEN;
	USART_init();
	PotsInit();

	while(1){
		TriggersResetAll();
		pause(1);
		Key(&keys[0]);
		Key(&keys[1]);
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
