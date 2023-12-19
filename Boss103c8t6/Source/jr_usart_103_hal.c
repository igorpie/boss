// 2018-11-14

/* библиотека работы с USART1 STM32F103
 * для отладочного вывода и грубого ввода
 *
 * Версия 0.01 от 04.12.2014
 *
 * 	USART_init() 	- инициализация
 * 	USART_PUTC(с) 	- передать символ
 * 	с=USART_GETC() - получить символ (с клавиатуры терминала, например)
 * 	USART_PRINT(s) - передача последовательности символов без перевода строки и возврата каретки
 * 	USART_PRINTLN(s) - передача последовательности символов с переводом строки и возвратом каретки
 * 	USART_CLS() 	- очистка экрана, курсор в позицию 0,0
 */

//#define _usart

#include "ProjectMain.h"
#include "jr_usart_103_hal.h"

#ifdef _usart
void USART_init(void){
	USART_CLS();
	debug(project_name);
	debug(version_string);
	debug(project_date);
	debug(project_time);
//	get_speed();mm
}


//*** Функция передачи символа ***
void USART_PUTC(char data) {
  while(!(USART2->SR & USART_SR_TC)); 							//Проверяем установку флага TC - завершения предыдущей передачи
  USART2->DR = data; 											//Записываем значение в регистр данных - передаем символ
}


/*** прием символа***/
char USART_GETC(void){
	char d=0;
	if((USART2->SR & USART_SR_RXNE)){ 							//Проверяем поступление данных от компьютера
		d = USART2->DR; 										//Считываем принятые данные
	}
	return d;
}


//*** Функция передачи строки через USART ***
void USART_PRINTLN(char* str){
  int i=0;
  while(str[i]) USART_PUTC(str[i++]);
}


void USART_PRINT(char* str){
  int i=0;
  while(str[i]) USART_PUTC(str[i++]);
}


void echo(char* str){	// == USART_PRINT
  int i=0;
  while(str[i]) USART_PUTC(str[i++]);
}


void USART_CLS(void){
	char CLS[]={0x1b,'[','2','J',0x1b,'[','H',0x0};
	USART_PRINT(CLS);
}


char *dec_u32(unsigned int i){									// возвращает стринг сделанный из INT
	static char str[11];
	char *s = str + sizeof(str);
	*--s = 0;													//end of string
	do	{
		*--s = '0' + (char)(i % 10);
		i /= 10;
	}
	while(i);
	return(s);
}


void debug(char *s){
	USART_PRINT("\n\rD: ");
	USART_PRINTLN(s);
}


void debug2(char *s, unsigned int i){
	USART_PRINT("\n\rD: ");
	USART_PRINT(s);
	USART_PRINTLN(dec_u32(i));
}

/*
void get_speed(void){

	RCC_ClocksTypeDef i;
	RCC_GetClocksFreq(&i);
	debug2("sys clock = " , i.SYSCLK_Frequency);
	debug2("HCLK = " , i.HCLK_Frequency);

	debug2("PCLK1 = " , i.PCLK1_Frequency);
	debug2("PCLK2 = " , i.PCLK2_Frequency);
	debug2("ADCCLK = " , i.ADCCLK_Frequency);

}
*/

char n2hex(char x){	// nibble 2 hex
	x &= 15;
	if (x > 9) x += 'A'-10;
	else x += '0';
	return x;
}


void dump_hex8(char *a, int l)	{								// массив байт, сколько байт выводить
	int i;

	for (i = 0 ; i < l; i++){									// главцикл
		if ((i%16) ==0) echo("\r\n");
		USART_PUTC( n2hex(a[i] >> 4) );
		USART_PUTC( n2hex(a[i]) );
		USART_PUTC( ' ' );
	}
	echo("\n\r");
}


void dump_hex8c(const char *a, int l)	{						// массив байт, сколько байт выводить
	int i;

	for (i = 0 ; i < l; i++){									// главцикл
		if ((i%16) ==0) echo("\n\r");
		USART_PUTC( n2hex(a[i] >> 4) );
		USART_PUTC( n2hex(a[i]) );
		USART_PUTC( ' ' );
	}
	echo("\n\r");
}


void dump_ascii8(char *a, int l)	{							// массив байт, сколько байт выводить
	int i;

	for (i = 0 ; i < l; i++){									// главцикл
		if ((i%16) ==0) echo("\n\r");
		USART_PUTC( (a[i]) );
	}
	echo("\n\r");
}


void echo_hex8(unsigned int a ) {
	a &= 255;
	USART_PUTC( n2hex(a >> 4) );
	USART_PUTC( n2hex(a) );
}


char *hex_u32(unsigned int i){									// возвращает стринг сделанный из INT
	static char str[9];
	int x;
	for (x = 7 ; x >= 0 ; x--) {
		str[x] = n2hex(i&15);
		i >>= 4;
	}
	str[8]=0;
	return(str);
}


void echo_hex32(unsigned int i){
	echo(hex_u32(i));
}
void echo_hex33(char *s, unsigned int i){
	echo(s);
	echo(hex_u32(i));
}



void crlf(void){
	echo("\n\r");
}


void echo_u32(unsigned int i){
	echo(dec_u32(i));
}


void echo_u33(char *s, unsigned int i){
	echo(s);
	echo(dec_u32(i));
}

void echo_s(char *s , char *s1){
	echo(s);
	echo(s1);
	crlf();
}


void echo_star(void){
	char i;
	crlf();crlf();
	for (i = 0 ; i < 60 ; i++) USART_PUTC('#');
	crlf();
}


void echo_float(char *s , float f){								// печать float 4 знака после запятой
	echo(s);
	echo(dec_float(f));
}


char *dec_float(float f){										// печать float 4 знака после запятой
	static char str[16];
	char signum = ' ';

	if (f<0) {
		f *= -1;
		signum = '-';
	}

	int t = (f-(int)f) * 10000.f;
	int i = f;
	int l;

	char *s = str + sizeof(str);
	*--s = 0;													//end of string

	for (l = 0 ; l < 4 ; l++){									// дробная часть
		*--s = '0' + (char)(t % 10);
		t /= 10;
	}
	while(t);

	*--s = '.';

	do {
		*--s = '0' + (char)(i % 10);
		i /= 10;
	}
	while(i);
	*--s = signum;
	return(s);
}


#endif
