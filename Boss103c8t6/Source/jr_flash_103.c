//   **************
//  **  FLASH   **
//**************

#include "jr_flash_103.h"
#include "ProjectMain.h"
#include "HelperLib.h"
#include "jr_usart_103_hal.h"

// Запись: разблокировать, стереть страницу, записывать (u16, если надо u32, то бить на 2 u16) , заблокировать
// Чтение: читать u16, если надо u32, то делать из пары u16

// разблокирование  (кирпичик)
void flash_unlock() {
	FLASH->KEYR = 0x45670123;
	FLASH->KEYR = 0xCDEF89AB;
}

// блокирование, чтобы никто не внедрил свой код и не слил прошивку  (кирпичик)
void flash_lock(){
	FLASH->CR |= FLASH_CR_LOCK;
}

// * Стирание страницы Flash 
void flash_erase(unsigned int pageAddress){	// pageAddress - любой адрес, принадлежащий стираемой странице
//	echo_hex33("\r\nErase adress: 0x", pageAddress);
	//led_sd_toggle();

    while (FLASH->SR & FLASH_SR_BSY);
    if (FLASH->SR & FLASH_SR_EOP)  FLASH->SR = FLASH_SR_EOP;

    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = pageAddress;
    FLASH->CR |= FLASH_CR_STRT;

    while (!(FLASH->SR & FLASH_SR_EOP));
    FLASH->SR = FLASH_SR_EOP;
    FLASH->CR &= ~FLASH_CR_PER;
}


// * запись u32 по заданному адресу  (кирпичик)
void jrflash_write_u32(unsigned int data, unsigned int address){
    unsigned int i;
    while (FLASH->SR & FLASH_SR_BSY);
    if (FLASH->SR & FLASH_SR_EOP)  FLASH->SR = FLASH_SR_EOP;

    FLASH->CR |= FLASH_CR_PG;

    for (i = 0; i < 4; i += 2) {
        *(volatile unsigned short*)(address + i) = (u16)data;			// INT 32 бита разбивается на 2 по 16 бит
        while (!(FLASH->SR & FLASH_SR_EOP));
        FLASH->SR = FLASH_SR_EOP;
        data >>= 16;
    }
    FLASH->CR &= ~(FLASH_CR_PG);
}


// чтение u32 из flash, кирпичик
unsigned int flash_read_u32(unsigned int a){
	u32 d;
	d= *(volatile unsigned int*) a;
	return(d);
}


// стирание страниц
// на входе первая страница (0...127) , число страниц
void flash_erase_buf(int start , int len){
	int i;
	flash_unlock();
	for ( i = start ; i < (start+len) ; i++ )	flash_erase( _page_size * i + FLASH_BASE );
	flash_lock();
}


// запись страницы из массива char во флэш
// на входе указатель на массив, номер страницы (0..127 для stm32f103cbt6)
void jrflash_write_page(char * s, unsigned int p){
    unsigned int i;
    int address;
    u16 d;

    address = FLASH_BASE + p*_page_size;
    flash_unlock();

    while (FLASH->SR & FLASH_SR_BSY);
    if (FLASH->SR & FLASH_SR_EOP)  FLASH->SR = FLASH_SR_EOP;
    FLASH->CR |= FLASH_CR_PG;


    for (i = 0; i < _page_size; i += 2) {
    	d =  *(volatile u16 *) s;
    	s = s + 2;
        *(volatile u16*)(address + i) = d;			// INT 32 бита разбивается на 2 по 16 бит
        while (!(FLASH->SR & FLASH_SR_EOP));
        FLASH->SR = FLASH_SR_EOP;
    }
    FLASH->CR &= ~(FLASH_CR_PG);

	flash_lock();
}


// копирование из ОЗУ в Flash
void FlashSave(){
	int len = kSizePreset * PRESETS_NUM;
	char *s = (char *)presets;
	unsigned int address = FLASH_BASE + kFlashPage * _page_size;

	u16 d;
	flash_unlock();
	flash_erase(address);

    while (FLASH->SR & FLASH_SR_BSY);
    if (FLASH->SR & FLASH_SR_EOP)  FLASH->SR = FLASH_SR_EOP;
    FLASH->CR |= FLASH_CR_PG;

    // состояния
    for (int i = 0; i < len; i += 2) {
    	d =  *(volatile u16 *) s;								// 2xChar to 1 x u16
    	s = s + 2;
        *(volatile u16*)(address + i) = d;						// int 32 бита разбивается на 2 по 16 бит
        while (!(FLASH->SR & FLASH_SR_EOP));
        FLASH->SR = FLASH_SR_EOP;
    }
    FLASH->CR &= ~(FLASH_CR_PG);

    jrflash_write_u32(presetNumber , address+1020);				// номер пресета в конце страницы

	flash_lock();
}


// копирование из Flash в ОЗУ
void FlashLoad(){						// len - любое
	LDIRc( (char *) (FLASH_BASE + kFlashPage * _page_size) , (char *)presets , kSizePreset * PRESETS_NUM);
	presetNumber = *(unsigned int *) (FLASH_BASE + kFlashPage * _page_size + 1020);
}
