#ifndef __USART2_H
#define __USART2_H

#include "stm32f10x.h"
#include "stdio.h"

#define USART2_BaudRate 115200

void USART2_Init(void);
void USART2_print_bin(int val2);

#endif
