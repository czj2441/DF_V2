#include "timer6.h"

u32 onPowerTime, countDownTime, lastDataTime, lastCANTime;//��¼�ϵ�����е�ʱ��

void TIM6_Init(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
    NVIC_InitTypeDef			NVIC_InitStructure;

    TIM_TimeBaseStructure.TIM_Period = 999;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//ʱ�Ӳ��ָ���벶���˲��ã�
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;//TIM6�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);//ʹ��TIM6�����ж�
    TIM_Cmd(TIM6, ENABLE);//ʹ��TIM6
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
