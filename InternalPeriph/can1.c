/**************************************************************************
 * @file     	boardA_exchange.c
 * @brief    	����can1���ߺ�A��ͨ�ţ���ӦA���can2
 * @writer		YSU��ӥս��2019�����
 **************************************************************************/
#include "can1.h"
extern u32 lastCANTime, onPowerTime;
struct ReceiveTypeDef motor_Feedback;
/************************************�м亯������***************************************************/
void Recieve_Sult(CanRxMsg *msg);
u8 Can_Send_Msg(u8* msg, u8 len);
/**************************************************************************************************/

/**
  * @name	Can1_Init()
  *
  * @brief 	Can1��ʼ����������Ϊ��A��ͨ�ŵ�����
  *
  * @brief 	����������˵��:
  *			32λ FilterId�д�������ݷֱ�Ϊ��STID[10:0]+EXTID[17:0]+IDE+RTR+0
  *			��������ֻ����STDIDΪ0x31X������ 12λ�����Ʊ�ʾΪ��0011 0001 XXXX
  *			FilterId�ĸ�11λӦ������Ϊ��0110 001X XXX������FilterIdֻ��STID����11λ������12λ��ʾ��SRID���λ��0��ȥ��
  *			��11λ��Ӧ������Ϊ��			   1111 1110 000
  *			FilterId��������Ϊ��0110 001X XXXX XXXX XXXX XXXX XXXX XXXX ����Ϊ 0110 0010 0000 0000 0000 0000 0000 0000(0x6200 0000)
  *			��Ӧ����Ϊ��	   1111 1110 0000 0000 0000 0000 0000 0000 (0xFE00 0000)
  *			��Ӧ������Ϊ��
  *			CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	//����λģʽ
  *			CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	//32λ��
  *			CAN_FilterInitStructure.CAN_FilterIdHigh=0x6200;				//32λID
  *			CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  *			CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xFE00;			//32λMASK
  *			CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
  *
  *@brief	����������˵����
  *			������������ܹ�����ͨ�ţ���Ҫ������ͬ�Ĳ����ʣ������ʼ��㹫ʽΪ(����Ƶ��/((bs1 + bs2 + 1)*��Ƶϵ��))
  *			ʱ��Ƶ�ʣ�����оƬ���ϻ��߿⺯��ע���в�ѯ��F103ZET6��ʱ��Ƶ��72MHZ can�����ڵ�APB1����2��Ƶ���������Ƶ��Ϊ36MHZ
  *			�涨�����ͨ��Ƶ��Ϊ1MHZ�����ݹ�ʽ�ó�(bs1 + bs2 + 1)*��Ƶϵ�� = 36 �����ж������÷�ʽ����ͬ���÷�ʽ����ʵ��ͨ�ţ������ȶ��Բ�ͬ�������������
  *			��Ƶϵ��Ϊ2 bs1Ϊ9 bs2Ϊ8
  *			��Ӧ������Ϊ��
  *			CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;
  *			CAN_InitStructure.CAN_BS1=CAN_BS1_9tq;
  *			CAN_InitStructure.CAN_BS2=CAN_BS2_8tq;
  *			CAN_InitStructure.CAN_Prescaler=2;
  *
  * @param  None
  *
  * @return None
  */
void CAN1_Init()
{
    /*����Ҫ�õ��Ľṹ��*/
    GPIO_InitTypeDef 		GPIO_InitStructure;
    CAN_InitTypeDef        	CAN_InitStructure;
    CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
    NVIC_InitTypeDef  		NVIC_InitStructure;

    /*ʹ��ʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��PORTAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);	//ʹ��CAN1ʱ��


    /* Configure CAN pin: RX */     // PA11
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     //
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure CAN pin: TX */   // PA12
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    CAN_DeInit(CAN1);
    /*CAN��Ԫ����*/
    CAN_InitStructure.CAN_TTCM = DISABLE;			//��ʱ�䴥��ͨ��ģʽ
    CAN_InitStructure.CAN_ABOM = DISABLE;			//����Զ����߹���
    CAN_InitStructure.CAN_AWUM = DISABLE;			//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
    CAN_InitStructure.CAN_NART = ENABLE;			//��ֹ�����Զ�����
    CAN_InitStructure.CAN_RFLM = DISABLE;		 	//���Ĳ�����,�µĸ��Ǿɵ�
    CAN_InitStructure.CAN_TXFP = DISABLE;			//���ȼ��ɱ��ı�ʶ������
    CAN_InitStructure.CAN_Mode = 0;				//ģʽ���ã� mode:0,��ͨģʽ;1,�ػ�ģʽ;

    /*���ò�����*/
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;		//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
    CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq; 		//Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
    CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;		//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
    CAN_InitStructure.CAN_Prescaler = 2;
    CAN_Init(CAN1, &CAN_InitStructure);			//��ʼ��CAN1

    /*���ù�����*/
    CAN_FilterInitStructure.CAN_FilterNumber = 0;	//������0
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; 	//����λģʽ
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; 	//32λ��
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;				//32λID
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;			//32λMASK
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;	//������0������FIFO0
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;				//���������0
    CAN_FilterInit(&CAN_FilterInitStructure);							//�˲�����ʼ��

    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);	//FIFO0��Ϣ�Һ��ж�����
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // �����ȼ�Ϊ1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**
  * @name	USB_LP_CAN1_RX0_IRQHandler()
  * @brief 	Can1�����жϷ�������ÿ�յ�һ�����ݽ���һ���ж�
  * @param  None
  */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
		lastCANTime = onPowerTime;
    CanRxMsg rx_message;
    if(CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET) //CAN_IT_FMP0
    {
        CAN_ClearITPendingBit(CAN1, CAN_IT_FF0);
        CAN_ClearFlag(CAN1, CAN_FLAG_FF0);
        CAN_Receive(CAN1, CAN_FIFO0, &rx_message);//��ȡ����
        switch(rx_message.StdId)
        {
        case 0x201:
            _2006_3510_08_dataRecieve(&motor_Feedback, &rx_message);
            break;
        }
    }
}

void CAN_Set_Motor_Speed(s16 cm1_iq, s16 cm2_iq, s16 cm3_iq, s16 cm4_iq)
{
    CanTxMsg TxMessage;
    TxMessage.StdId = 0x200;
    TxMessage.IDE = CAN_Id_Standard;
    TxMessage.RTR = CAN_RTR_Data;
    TxMessage.DLC = 0x08;
    TxMessage.Data[0] = (uint8_t)(cm1_iq >> 8);//0x201 id:1
    TxMessage.Data[1] = (uint8_t)cm1_iq;
    TxMessage.Data[2] = (uint8_t)(cm2_iq >> 8);
    TxMessage.Data[3] = (uint8_t)cm2_iq;
    TxMessage.Data[4] = (uint8_t)(cm3_iq >> 8);
    TxMessage.Data[5] = (uint8_t)cm3_iq;
    TxMessage.Data[6] = (uint8_t)(cm4_iq >> 8);
    TxMessage.Data[7] = (uint8_t)cm4_iq;
    CAN_Transmit(CAN1, &TxMessage);
}
