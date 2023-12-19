// 2018-11-14
#include "main.h"


//void echo_long(unsigned long long );
void USART_init(void);
void USART_PUTC(char );
char USART_GETC(void);
void USART_PRINTLN(char* );
void USART_PRINT(char* );
void echo(char* );						// == USART_PRINT
void USART_CLS(void);
//char *dec_u64(unsigned long long );				// возвращает стринг сделанный из INT
char *dec_u32(unsigned int );				// возвращает стринг сделанный из INT
void debug(char *);
//void debug2(char *, unsigned long long );
void debug2(char *, unsigned int );
void get_speed(void);
void dump_hex8(char *, int );
void dump_hex8c(const char *, int );
void dump_ascii8(char *, int );
void debug_state(void);
void crlf(void);
void echo_u32(unsigned int );
void echo_u33(char *, unsigned int );
void echo_star(void);
void echo_s(char *, char *);

void echo_hex8(unsigned int a ) ;
char *hex_u32(unsigned int i);
void echo_hex32(unsigned int i);
void echo_hex33(char *s, unsigned int i);

char *dec_float(float f);
void echo_float(char *s , float f);

