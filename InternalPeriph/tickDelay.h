#ifndef __TICKDELAY_H
#define __TICKDELAY_H

#include "STM32F10x.h"

void SysTick_Delay_Init(void);
void SysTick_Delay_Ms(u16 nms);
void SysTick_Delay_Us(u32 nus);

#endif
