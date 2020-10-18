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
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3

    USART_InitStructure.USART_BaudRate = USART2_BaudRate;	//���������ã�115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//����λ�����ã�8λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1; 	//ֹͣλ���ã�1λ
    USART_InitStructure.USART_Parity = USART_Parity_No ;  //�Ƿ���żУ�飺��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//Ӳ��������ģʽ���ã�û��ʹ��
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�����뷢�Ͷ�ʹ��
    USART_Init(USART2, &USART_InitStructure);  //��ʼ��USART2
    USART_Cmd(USART2, ENABLE);// USART2ʹ��
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2 ;//��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//��Ӧ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);//����ָ���Ĳ�����ʼ��NVIC�Ĵ���
}

/* �������ض���c�⺯��printf��USART2
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
			case 0x05:ctrlMode = 2;break;//����
			case 0x06:ctrlMode = 1;break;//����
		}
}
