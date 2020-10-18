#ifndef __TIMER6_H
#define __TIMER6_H

#include "stm32f10x.h"
#include "timerCTRLtask.h"

void TIM6_Init(void);//TIM3作为分时控制的定时器，每1ms进入一次中断
void delay_ms(u16 ms);//延时函数

#endif
