#include "main.h"


#define kFlashPage 63											// номер страницы флэша для хранения всякого
#define _page_size 1024												// stm32f103cbt6 md flash page size


void flash_unlock();												// разблокирование  (кирпичик)
void flash_lock();													// блокирование, чтобы никто не внедрил свой код и не слил прошивку  (кирпичик)
void flash_erase(unsigned int pageAddress);							// Стирание страницы Flash , pageAddress - любой адрес, принадлежащий стираемой странице

void jrflash_write_u32(unsigned int data, unsigned int address);	// запись u32 по заданному адресу  (кирпичик)
unsigned int flash_read_u32(unsigned int a);						// чтение u32 из flash, кирпичик


void flash_erase_buf(int , int);									// стирание страниц фаза-1
void jrflash_write_page(char * s, unsigned int p);

void FlashSave();							// Запись массива произвольной длины в фиксированную страницу конфига
void FlashLoad();							// Чтение из страницы конфига флэш в ОЗУ
