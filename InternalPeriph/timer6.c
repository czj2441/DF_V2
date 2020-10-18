#include "timer6.h"

u32 onPowerTime, countDownTime, lastDataTime, lastCANTime;//记录上电后运行的时间

void TIM6_Init(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
    NVIC_InitTypeDef			NVIC_InitStructure;

    TIM_TimeBaseStructure.TIM_Period = 999;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//时钟不分割（输入捕获滤波用）
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;//TIM6中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);//使能TIM6更新中断
    TIM_Cmd(TIM6, ENABLE);//使能TIM6
}

void TIM6_IRQHandler(void) {
    if(TIM_GetITStatus(TIM6, TIM_IT_Update) == SET) {
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
        onPowerTime++;
        timeTaskControl();
    }
}

void delay_ms(u16 ms) {
    u32 start_time = onPowerTime;
    while(1) {
        if(onPowerTime > start_time + ms) break;
    }
}
