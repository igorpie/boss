#include "ProjectMain.h"
#include "debug.h"
#include "HelperLib.h"
#include "jr_flash_103.h"
#include "jr_usart_103_hal.h"
#include "ssd1306_tests.h"

DevStruct dev;
KeyStruct keys[] = {
		{ SW_1_Pin , GPIOA , 0 , 0 , 0 , 0   },		// A1
		{ SW_2_Pin , GPIOB , 0 , 0 , 0 , 0   },		// B2
};

uint16_t Adc1ConvertedValue[ADC_NUM_CHANNELS];
uint16_t Adc1Values[ADC_NUM_CHANNELS][kNumMeasure];

/* Потенциометры [0..3] и светодиодные входы [4..5] заведеные в АЦП
 * Потенциометры сохраняются во флэш.
 * Светодиоды просто удобно отфильтровать и по триггеру убедиться что есть изменения.
 */
PotStruct pots[ADC_NUM_CHANNELS];
int isChanged = kReset;											// bool , если крутнули ручку, то появляется "звездочка" призывающая к записи
int isNeedReload = kReset;										// bool , требуется загрузить пресет с текущим номером (из-за миди)


void ProjectMain(void){
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;							// пуск таймера
	TIM4->CR1 |= TIM_CR1_CEN;
	USART_init();
	PotsInit();
	keys[0].flag_release_short = kReset;
	keys[1].flag_release_short = kReset;
	keys[1].flag_hold_very_longer = kReset;
	keys[1].flag_release_very_longer = kReset;
	//ssd1306_TestAll();
//	TestFlash();												// uncomment for test

	FlashLoad((char *) pots, kSizePot*(ADC_NUM_CHANNELS-2));	// load settings from flash
	//todo:  initial digital pots setting() / make load preset
	TriggersResetAll();

	// * Главный цикл *
	while(1){
		pause(1);

		// * опрос и обработка кнопок *
		Key(&keys[0]);
		Key(&keys[1]);
		if (keys[0].flag_release_short == kSet) {
			keys[0].flag_release_short = kReset;
			if (--PresetNumber < 0 ) PresetNumber = 9;
			isNeedReload = kSet;

		}
		if (keys[1].flag_release_short == kSet) {
			keys[1].flag_release_short = kReset;
			PresetNumber++;
			PresetNumber %= 10;
			isNeedReload = kSet;
		}
		if (keys[1].flag_hold_very_longer == kSet) {
			// запись во флэш. состояние входов лампочек не сохраняется
			FlashSave((char *) pots, kSizePot*(ADC_NUM_CHANNELS-2));
			debug("Save to flash OK");

			while(!keys[1].flag_release_very_longer)			// ожидание отпускания кнопки, иначе запись во флэш будет в цикле
				Key(&keys[1]);

			keys[1].flag_release_very_longer = kReset;
			keys[1].flag_hold_very_longer = kReset;
			isChanged = kReset;
			// todo: clean "*" on the screen
		}

		// * обработка потенциометров *
		ScanPotsShadow();
		for (int i = 0 ; i < ADC_NUM_CHANNELS ; i++) {
			if (pots[i].trig[0] == kSet){
				pots[i].trig[0] = kReset;
				isChanged = kSet;								// крутнули ручку или что-то прилетело по миди.
				//todo: set "*" on the screen
				isNeedReload = kSet;
				debugAdc();
			}
		}


		// todo * обработка/байпас лампочек *


		// * загрузка пресета если надо *
		if (isNeedReload) {
			isNeedReload = kReset;
			PresetLoad();
		}
	}
};


// загрузка в цифровые поты текущих значений
void PresetLoad(void){
	debugState();
	//todo: load preset
	//todo: display
}


// Миди прикидывается потенциометрами для СС и кнопками для РС
void UsbReceivedMidiCC(int byte1 , int byte2 , int byte3){
	debug2("USB MIDI CC: " , byte1);
	debug2("USB MIDI Controller: " , byte2);
	debug2("USB MIDI Value: " , byte3);

	/* гугль хром при перезагрузке страницы сыплет сообщениями СС
	 * в контроллеры 123 и 121 в разные миди каналы.
	 * их - пропускаем
	 */
	if ( byte2 > 99)
		return;

	// контроллеры [1..4] и кратные им переводятся в потенциометры 0..3. Проще настраивать.
	int pot = (byte2 - 1) % 4;
	pots[pot].val_int[0] = byte3 * 2;
	pots[pot].trig[0] = kSet;

};

void UsbReceivedMidiPC(int byte1 , int byte2){
	debug2("USB MIDI PC: " , byte1);
	debug2("USB MIDI Value: " , byte2);
	PresetNumber = byte2 % 10;
	isNeedReload = kSet;										// load preset
};

