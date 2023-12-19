/*
 * HelperLib.h
 *
 *      Author: 1
 */


#ifndef HELPERLIB_H_
#define HELPERLIB_H_
#include "ProjectMain.h"
typedef unsigned int u32;

	// * структура информации о потенциометре. 8 32битных слов + 4х4= 16 слов => 24 слов на 1 потенциометр или аналоговый вход или 96 байт
	typedef struct {
		int	adc;											// целое медианное значение, после nx
		float f1;											// фильтр (текущий заряд кондера)
		float cap;											// емкость кондера / бленд. Может меняться, если крутнут ручку.
		int	i;												// счетчик. Если >0 , значит недавно крутили ручку.
		float f2;											// 0..1023 второй фильтр
		float f2_old;
		// без удержания кнопок, с удержанием левой, правой, обеих
		float	unfiltered;
		float	val_int[4];									// значение, можно использовать.  без, байпас, тап, обе
		float	val_temp;									// временное значение, нельзя использовать.

		float	val[4];										// без, байпас, тап, обе
		float	val_old[4];
//		float	val_polished[4];
		int		trig[4];									// триггер, что обнаружено кручение ручки
	} PotStruct;



// * кнопки *

	// структура информации о нажатой кнопке/переключателе/ноге энкодера. Используется с RDFX фильтром. Состояние вкл/выкл
	typedef struct {
		uint32_t pin;										// номер вывода порта А для опроса (процедура написана под любой пин, но порта А)
		GPIO_TypeDef * gp;									// не активно ** лень разбираться с GPIOB->IDR = GPIOB->IDR
		int value;											// значение скана кнопки (0 или 1 умноженное на 256)
		int filter;											// значение фильтра
		int filter_old;										// значение предыдущее для фильтра (на всякий)
		int state;											// итоговое фильтрованное состояние для работы 0 или 1.
		int state_old;										// итоговое фильтрованное состояние для работы 0 или 1.
	} Key2Struct;



	// структура о нажатой кнопке с автоматом защиты от дребезга и кучей состояний.
	typedef struct {										// структура информации о нажатой кнопке
		// защита от дребезга и состояния клавиши. Обрабатывается JR_KEY
		u32 pin;											// номер вывода порта А для опроса (процедура написана под любой пин, но порта А)
		GPIO_TypeDef * gp;
		int state;											// результат последнего опроса (4 - первый клик, 0 - ничего не произошло и тп)
		int state_old;										// результат предпоследнего опроса
		int drebezg_counter;								// счетчик дребезга нажатия
		int drebezg_release;								// счетчик дребезга отпускания

		// виды нажатий для тапа и разного - семафоры. Обычно значения 0, 1, реже -1. Обрабатывается много где. Флаги сбрасываются вручную
		int flag_first_click;								// если событие 4, то флаг = 1. Иначе - 0.
		int flag_double_click;								// (4 новое минус 4 старое) < 0.5 сек
		int flag_hold_long;									// hold
		int flag_hold_longer;								// hold держут
		int flag_hold_very_longer;								// hold держут еще дольше (3+ сек)
		int flag_release_very_longer;						// hold держали дольше (3+ сек) и отпустили
		int flag_release_short;								// быстрое отжатие
		int flag_release_long;								// отжатие после долгого нажатия
		unsigned int time;									// время нажатия
		unsigned int time_old;								// double click controlled время предыдущего клика,

		unsigned int time_release;							// время отпускания

	}  KeyStruct;


	enum key_states {
		key_nothing = 0 ,			// * 0  - ничего не нажато
		key_first_press ,			// * 4  - первое нажатие (надо для TAP, чтобы среагировать и снять счетчик. Неизвестно будет нажатие длинным, или коротким, событие до отпускания
		key_release_short ,			// * 1  - отпускание после короткого нажатия,  можно реагировать на клик (кнопку уже отпустили)
		key_hold_short ,			// * 15  - нажатие больше короткого, но меньше длинного. Кнопка прошла проверку на дребезг и её держат дальше. (кнопку пока не отпустили)
		key_hold_long ,				// * 20 - длинное нажатие, клавишу держат, можно реагировать на длинный клик, допустим 300мс
		key_hold_longer ,			// * 21 - длинное нажатие, клавишу держат дальше после события 20. допустим 300мс+
//		key_hold_very_long ,		// очень длинное нажатие, допустим > 3 секунд
		key_release_long ,			// * 23 - длинное нажатие, клавишу только отпустили и событие 20 и 21 до этого "давно" было.
		key_bounce_release ,		// * 66  - состояние дребезга отпускания
		key_bounce_press			// * 67  - пропуск дребезга нажатия
	};

	// тайминги клавиш, в миллисекундах (заточены под 1000 прерываний в секунду (часы = 1кГц) и внутренние счетчики).
	#define LONG_PRESS 		(300)									// длинное нажатие - N интов таймера
	#define VERY_LONG_PRESS (3000)									// длинное нажатие - N интов таймера
	#define DREBEZG			(130)									// время дребезга при нажатии (с запасом)  сек
	#define DREBEZG_RELEASE	(130)									// время дребезга при отпускании
	#define DOUBLE_CLICK	(500)									// время на двойной клик
	#define SINGLE_CLICK	(300)									// время на одиночный клик (200мс считается за 1 нажатие). оказалось важно чтобы он был больше 12000, видимо,


void halt(void);
void pause(int v);
void ScanPotsShadow(void);
void PotsInit(void);
void TriggersResetAll(void);
void Key(KeyStruct * k);

void LDIR(char * , char * , int );								// побайтовое копирование
void LDIRc(const  char * ,char * , int );
void CLEARS(char * , int );										// очистка


#endif /* HELPERLIB_H_ */