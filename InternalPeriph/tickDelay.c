#include "tickDelay.h"

static u8  us_period=0;
static u16 ms_period=0;

void SysTick_Delay_Init()
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
    us_period=SystemCoreClock/8000000;	//每us需要的systick时钟数
    ms_period=(u16)us_period*1000;//每us需要的systick时钟数
}

void SysTick_Delay_Us(u32 nus)
{
    u32 temp;
    SysTick->LOAD=nus*us_period;//设定定时器计数
    SysTick->VAL=0x00;//清空已走数值
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//使能Systick
    do {
        temp=SysTick->CTRL;
    } while((temp&0x01)&&!(temp&(1<<16)));//判断Systick是否仍然在运行并且还没有计数到0
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//失能计数器
    SysTick->VAL =0X00;//清空已走数值
}

void SysTick_Delay_Ms(u16 nms)//72M条件下,nms<=1864
{
    u32 temp;
    SysTick->LOAD=nms*ms_period;//设定定时器计数
    SysTick->VAL=0x00;//清空已走数值
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//使能Systick
    do {
        temp=SysTick->CTRL;
    } while((temp&0x01)&&!(temp&(1<<16)));//判断Systick是否仍然在运行并且还没有计数到0
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//失能计数器
    SysTick->VAL =0X00;//清空已走数值
}
