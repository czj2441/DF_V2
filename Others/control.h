#ifndef CONTROL_H
#define CONTROL_H

#include <stdlib.h>
#include "stm32f10x.h"
#include "timer6.h"
#include "WS2812B.h"
#include "hx711.h"

void ledCtrl(u8 ledID, u8 ledMode);
void ledOFF(void);
void ledAdd(void);
void ledReborn(void);
void ledFlash(void);
void checkSensors(void);

#endif
