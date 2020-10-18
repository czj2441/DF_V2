#include "controlSignal.h"

void onBoardLedInit() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    for(u8 t=0; t<5; t++) {
        GPIO_SetBits(GPIOB, GPIO_Pin_10);
        GPIO_SetBits(GPIOB, GPIO_Pin_11);
        delay_ms(50);
        GPIO_ResetBits(GPIOB, GPIO_Pin_10);
        GPIO_ResetBits(GPIOB, GPIO_Pin_11);
        delay_ms(50);
    }
}
