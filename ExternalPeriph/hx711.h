#ifndef __HX711_H
#define __HX711_H

#include "stm32f10x.h"
#include "timer6.h"
#include "basicMath.h"

void hx711_Init(void);
void hx711_readAll(u8 n);
void hx711_Detect(void);
struct hx711Info{
	GPIO_TypeDef * DT_Port;
	u16 DT_Pin;
	GPIO_TypeDef * SCK_Port;
	u16 SCK_Pin;
	
	long long basicNum;//传感器初始数值
	long long num1;
	long long num2;
	long long oldNumAvg;
	long long numAvg;
};

#endif
