#ifndef __WS2812B_H
#define	__WS2812B_H

#include "stm32f10x.h"

void ws2812Init(void);
void ws2812Ctrl(u8 nled, uint8_t (*color)[3], uint16_t len);

void ws2812_led1_send(uint8_t (*color)[3], uint16_t len);
void ws2812_led2_send(uint8_t (*color)[3], uint16_t len);
void ws2812_led3_send(uint8_t (*color)[3], uint16_t len);
void ws2812_led4_send(uint8_t (*color)[3], uint16_t len);
void ws2812_led5_send(uint8_t (*color)[3], uint16_t len);

#endif
