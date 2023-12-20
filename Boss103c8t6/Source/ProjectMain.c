#include "ProjectMain.h"
#include "debug.h"
#include "HelperLib.h"
#include "jr_flash_103.h"
#include "jr_usart_103_hal.h"
#include "ssd1306.h"


KeyStruct keys[] = {
		{ SW_1_Pin , GPIOA , 0 , 0 , 0 , 0   },		// A1
		{ SW_2_Pin , GPIOB , 0 , 0 , 0 , 0   },		// B2
};

/* Потенциометры [0..3] и светодиодные входы [4..5] заведеные в АЦП
 * Все фильтруется
 * Потенциометры сохраняются в флэш.
 * Светодиоды чтобы триггеру убедиться что есть изменения.
 */
uint16_t Adc1ConvertedValue[ADC_NUM_CHANNELS];
uint16_t Adc1Values[ADC_NUM_CHANNELS][kNumMeasure];


DevStruct dev = {.test = "info"};
unsigned int presetNumber = 7;									// номер текущего пресета
PotStruct pots[ADC_NUM_CHANNELS] = {0};							// состояние текущего пресета
PotStruct presets[PRESETS_NUM][ANALOG_POT_ADC_NUM] = {0};		// 2d массив потенциометров под пресеты. ~192 байта х число пресетов

int isChanged = kReset;											// bool , если крутнули ручку, то появляется "звездочка" призывающая к записи
int isNeedReload = kReset;										// bool , требуется загрузить пресет с текущим номером (из-за миди)


/* ---------------------------------------------------------- */
/*                           main()                           */
/* ---------------------------------------------------------- */
void ProjectMain(void){
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;							// пуск таймера
	TIM4->CR1 |= TIM_CR1_CEN;
	USART_init();
	ssd1306_Init();

	PotsInit();
	keys[0].flag_release_short = kReset;
	keys[1].flag_release_short = kReset;
	keys[1].flag_hold_very_longer = kReset;
	keys[1].flag_release_very_longer = kReset;


//	TestFlash();												// uncomment for test
	FlashLoad();												// load settings from flash to RAM
	isNeedReload = kSet;										// принудительная загрузка пресета из ОЗУ в текущие параметры
	TriggersResetAll();

	// * Главный цикл *
	while(1){
		// * загрузка пресета если надо *
		if (isNeedReload) {
			isNeedReload = kReset;
			PresetLoadFromRam();
		}
		pause(1);

		// * опрос и обработка кнопок *
		Key(&keys[0]);
		Key(&keys[1]);
		if (keys[0].flag_release_short == kSet) {
			keys[0].flag_release_short = kReset;
			if (--presetNumber > PRESETS_NUM ) presetNumber = PRESETS_NUM - 1;
			isNeedReload = kSet;
			isChanged = kReset;

		}
		if (keys[1].flag_release_short == kSet) {
			keys[1].flag_release_short = kReset;
			presetNumber++;
			presetNumber %= PRESETS_NUM;
			isNeedReload = kSet;
			isChanged = kReset;
		}
		if (keys[1].flag_hold_very_longer == kSet) {
			// запись во флэш. состояние входов лампочек не сохраняется
			FlashSave((char *) presets, kSizePreset * PRESETS_NUM);
			debug("Save to flash OK");

			isChanged = kReset;
			display();											// clean "*" on the screen

			while(!keys[1].flag_release_very_longer)			// ожидание отпускания кнопки, иначе запись во флэш будет в цикле
				Key(&keys[1]);
			keys[1].flag_release_very_longer = kReset;
			keys[1].flag_hold_very_longer = kReset;
		}

		// * обработка потенциометров *
		ScanPotsShadow();
		for (int i = 0 ; i < ANALOG_POT_ADC_NUM ; i++) {
			if (pots[i].trig[0] == kSet){						// крутнули ручку или что-то прилетело по миди.
				pots[i].trig[0] = kReset;
				isChanged = kSet;								// установка "*"
				PresetSaveToRam();								// сохранение изменений в массив параметров
				isNeedReload = kSet;							// флаг загрузки параметров из ОЗУ в текущий пресет и в цифровые потенциометрыы
				debugAdc();
			}
		}

		// todo * обработка входов лампочек *
	}
};


// сохранение текущего пресета в массив в ОЗУ
void PresetSaveToRam(void){
	for (int i = 0 ; i  < ANALOG_POT_ADC_NUM ; i++)
		presets[presetNumber][i].val_int[0]= pots[i].val_int[0];
}

// загрузка в текущий пресет из массива в ОЗУ. Применение = Загрузка цифровых потов
void PresetLoadFromRam(void){
	debugState();
	for (int i = 0 ; i  < ANALOG_POT_ADC_NUM ; i++)				//load preset data from RAM
		pots[i].val_int[0] = presets[presetNumber][i].val_int[0];

	//todo: set states to digital pots and(?) leds
	display();													//display
}


// Миди прикидывается потенциометрами для СС и кнопками для РС
void UsbReceivedMidiCC(int byte1 , int byte2 , int byte3){
	/* гугль хром при перезагрузке страницы сыплет сообщениями СС
	 * в контроллеры 123 и 121 в разные миди каналы.
	 * их - пропускаем
	 */
	if ( byte2 > 99) {
		debug2("Ignored ", byte2);
		return;
	}
	debug2("USB MIDI CC: " , byte1);
	debug2("USB MIDI Controller: " , byte2);
	debug2("USB MIDI Value: " , byte3);

	// контроллеры [1..4] и кратные им переводятся в потенциометры 0..3. Проще настраивать.
	int pot = (byte2 - 1) % 4;
	pots[pot].val_int[0] = byte3 * 2;
	pots[pot].trig[0] = kSet;

};

void UsbReceivedMidiPC(int byte1 , int byte2){
	debug2("USB MIDI PC: " , byte1);
	debug2("USB MIDI Value: " , byte2);
	presetNumber = byte2 % 10;
	isChanged = kReset;
	isNeedReload = kSet;										// load preset
};

void display(void){
	ssd1306_Fill(Black);										// CLS

	ssd1306_SetCursor(32  , 4);
	ssd1306_WriteString("Inputs 0..4", Font_7x10, White);

	int x = 4*7;
	for (int i = 0 ; i  < ANALOG_POT_ADC_NUM ; i++) {
		ssd1306_SetCursor(16 + x * i , 18);
		ssd1306_WriteString(dec_u32(pots[i].val_int[0]), Font_7x10, White);
	}

	ssd1306_SetCursor(4 , 8);									// "*" вперед чтобы не наложилась на номер пресета
	if (isChanged)
		ssd1306_WriteChar('*' , Font_11x18 , White);

	ssd1306_SetCursor(0 , 14);									// номер пресета
	ssd1306_WriteChar((presetNumber & 7) + '0' , Font_11x18 , White);


	for (int i = 0 ; i  < ANALOG_POT_ADC_NUM ; i++) {			// слайдеры
		int x1 = 15 + x * i;
		int dx = x1 + pots[i].val_int[0] / 11 + 1;
		ssd1306_Line(x1, 30, dx,  30, White);
		ssd1306_Line(x1, 31, dx,  31, White);
	}
	ssd1306_UpdateScreen();
}
