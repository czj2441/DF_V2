#include "usart2.h"

extern u8 ctrlMode, enAct, enActChanged;
extern u32 onPowerTime, lastDataTime;

void USART2_Init() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef 	GPIO_InitStructure;
    USART_InitTypeDef 	USART_InitStructure;
    NVIC_InitTypeDef 	NVIC_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3

    USART_InitStructure.USART_BaudRate = USART2_BaudRate;	//波特率设置：115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//数据位数设置：8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1; 	//停止位设置：1位
    USART_InitStructure.USART_Parity = USART_Parity_No ;  //是否奇偶校验：无
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//接收与发送都使能
    USART_Init(USART2, &USART_InitStructure);  //初始化USART2
    USART_Cmd(USART2, ENABLE);// USART2使能
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2 ;//抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);//根据指定的参数初始化NVIC寄存器
}

/* 描述：重定向c库函数printf到USART2
int fputc(int ch, FILE *f){
    USART_SendData(USART2, (unsigned char) ch);
    while (!(USART2->SR & USART_FLAG_TXE));

    return (ch);
}*/

void USART2_IRQHandler(void)
{
    USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		lastDataTime = onPowerTime;
		u16 recv = USART_ReceiveData(USART2);
		switch(recv){
			case 0x00:enAct = 0;enActChanged = 1;break;
			case 0x01:enAct = 1;enActChanged = 1;break;
			case 0x05:ctrlMode = 2;break;//击中
			case 0x06:ctrlMode = 1;break;//击错
		}
}
