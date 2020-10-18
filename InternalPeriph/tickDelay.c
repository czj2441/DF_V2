#include "tickDelay.h"

static u8  us_period=0;
static u16 ms_period=0;

void SysTick_Delay_Init()
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
    us_period=SystemCoreClock/8000000;	//ÿus��Ҫ��systickʱ����
    ms_period=(u16)us_period*1000;//ÿus��Ҫ��systickʱ����
}

void SysTick_Delay_Us(u32 nus)
{
    u32 temp;
    SysTick->LOAD=nus*us_period;//�趨��ʱ������
    SysTick->VAL=0x00;//���������ֵ
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//ʹ��Systick
    do {
        temp=SysTick->CTRL;
    } while((temp&0x01)&&!(temp&(1<<16)));//�ж�Systick�Ƿ���Ȼ�����в��һ�û�м�����0
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//ʧ�ܼ�����
    SysTick->VAL =0X00;//���������ֵ
}

void SysTick_Delay_Ms(u16 nms)//72M������,nms<=1864
{
    u32 temp;
    SysTick->LOAD=nms*ms_period;//�趨��ʱ������
    SysTick->VAL=0x00;//���������ֵ
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//ʹ��Systick
    do {
        temp=SysTick->CTRL;
    } while((temp&0x01)&&!(temp&(1<<16)));//�ж�Systick�Ƿ���Ȼ�����в��һ�û�м�����0
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//ʧ�ܼ�����
    SysTick->VAL =0X00;//���������ֵ
}
