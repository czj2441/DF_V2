#include "usart1.h"

void USART1_Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE); //ʹ��GPIOA USART1ʱ��

    GPIO_InitTypeDef 	GPIO_InitStructure;
    USART_InitTypeDef 	USART_InitStructure;
    NVIC_InitTypeDef 	NVIC_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = USART1_BaudRate;	//���������ã�115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//����λ�����ã�8λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1; 	//ֹͣλ���ã�1λ
    USART_InitStructure.USART_Parity = USART_Parity_No ;  //�Ƿ���żУ�飺��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//Ӳ��������ģʽ���ã�û��ʹ��
    USART_InitStructure.USART_Mode = USART_Mode_Rx;//�����뷢�Ͷ�ʹ��
    USART_Init(USART1, &USART_InitStructure);  //��ʼ��USART2
    USART_Cmd(USART1, ENABLE);// USART2ʹ��
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2 ;//��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//��Ӧ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);//����ָ���Ĳ�����ʼ��NVIC�Ĵ���
}

void USART1_IRQHandler(void)
{
    USART_ClearITPendingBit(USART1,USART_IT_RXNE);
}