/*
 * ProjectMain.h
 */
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"



#ifndef PROJECTMAIN_H_
#define PROJECTMAIN_H_
#include "HelperLib.h"
	#define _usart 1												// сборка с USART RX и отладкой

	#define project_name "Controller "
	#define version_string "ver: 2023-06-12"
	#define project_date __DATE__
	#define project_time __TIME__

	#define ADC_NUM_CHANNELS	6
	#define kNumMeasure 16

	typedef unsigned int u32;
	typedef uint16_t u16;

	enum key_flags { kReset = 0 , kSet , kTristate  };							// состояние "семафоров" клавиш
	enum _states { kOff = 0, kOn  };
	enum btn_states { kPressed = 0 , kNot_pressed  };

	typedef struct {
		unsigned int clock;
		unsigned int changes;
	} DevStruct;

	#define PresetNumber pots[0].trig[3]							// unsigned int PresetNumber = 7;	// костыль: номер пресета сохраняется в неиспользуемой ячейке потенциометра, так проще писать во флэш.

	void ProjectMain(void);
	void PresetLoad(void);
	void UsbReceivedMidiCC(int byte1 , int byte2 , int byte3);
	void UsbReceivedMidiPC(int byte1 , int byte2);

	extern DevStruct dev;
	extern uint16_t Adc1ConvertedValue[ADC_NUM_CHANNELS];
	extern uint16_t Adc1Values[ADC_NUM_CHANNELS][kNumMeasure];
	extern PotStruct pots[ADC_NUM_CHANNELS];
	extern int isChanged;
#endif /* PROJECTMAIN_H_ */
