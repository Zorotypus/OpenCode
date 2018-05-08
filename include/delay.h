#ifndef DELAY_H_
#define DELAY_H_

#include "stm32f10x.h"

void delay_init();
void delay_ms(int ms);
void delay_ms(int us);
#endif
