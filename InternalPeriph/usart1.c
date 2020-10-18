#include "usart1.h"

void USART1_Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE); //使能GPIOA USART1时钟

    GPIO_InitTypeDef 	GPIO_InitStructure;
    USART_InitTypeDef 	USART_InitStructure;
    NVIC_InitTypeDef 	NVIC_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = USART1_BaudRate;	//波特率设置：115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//数据位数设置：8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1; 	//停止位设置：1位
    USART_InitStructure.USART_Parity = USART_Parity_No ;  //是否奇偶校验：无
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
    USART_InitStructure.USART_Mode = USART_Mode_Rx;//接收与发送都使能
    USART_Init(USART1, &USART_InitStructure);  //初始化USART2
    USART_Cmd(USART1, ENABLE);// USART2使能
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2 ;//抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);//根据指定的参数初始化NVIC寄存器
}

void USART1_IRQHandler(void)
{
    USART_ClearITPendingBit(USART1,USART_IT_RXNE);
}
