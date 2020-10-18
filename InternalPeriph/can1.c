/**************************************************************************
 * @file     	boardA_exchange.c
 * @brief    	利用can1总线和A板通信，对应A板的can2
 * @writer		YSU燕鹰战队2019电控组
 **************************************************************************/
#include "can1.h"
extern u32 lastCANTime, onPowerTime;
struct ReceiveTypeDef motor_Feedback;
/************************************中间函数声明***************************************************/
void Recieve_Sult(CanRxMsg *msg);
u8 Can_Send_Msg(u8* msg, u8 len);
/**************************************************************************************************/

/**
  * @name	Can1_Init()
  *
  * @brief 	Can1初始化函数，作为与A板通信的总线
  *
  * @brief 	过滤器设置说明:
  *			32位 FilterId中储存的内容分别为：STID[10:0]+EXTID[17:0]+IDE+RTR+0
  *			因此如果想只接受STDID为0x31X的数据 12位二进制表示为：0011 0001 XXXX
  *			FilterId的高11位应该设置为：0110 001X XXX（由于FilterId只给STID留了11位，所以12位表示的SRID最高位的0舍去）
  *			高11位对应的掩码为：			   1111 1110 000
  *			FilterId可以设置为：0110 001X XXXX XXXX XXXX XXXX XXXX XXXX 可以为 0110 0010 0000 0000 0000 0000 0000 0000(0x6200 0000)
  *			对应掩码为：	   1111 1110 0000 0000 0000 0000 0000 0000 (0xFE00 0000)
  *			对应的设置为：
  *			CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	//屏蔽位模式
  *			CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	//32位宽
  *			CAN_FilterInitStructure.CAN_FilterIdHigh=0x6200;				//32位ID
  *			CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  *			CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xFE00;			//32位MASK
  *			CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
  *
  *@brief	波特率设置说明：
  *			两个板子如果能够正常通信，需要设置相同的波特率，波特率计算公式为(总线频率/((bs1 + bs2 + 1)*分频系数))
  *			时钟频率：可在芯片资料或者库函数注释中查询，F103ZET6的时钟频率72MHZ can所挂在的APB1总线2分频，因此总线频率为36MHZ
  *			规定两板的通信频率为1MHZ，根据公式得出(bs1 + bs2 + 1)*分频系数 = 36 可以有多种设置方式，不同设置方式都能实现通信，但是稳定性不同，这里给出的是
  *			分频系数为2 bs1为9 bs2为8
  *			对应的设置为：
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
    /*声明要用到的结构体*/
    GPIO_InitTypeDef 		GPIO_InitStructure;
    CAN_InitTypeDef        	CAN_InitStructure;
    CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
    NVIC_InitTypeDef  		NVIC_InitStructure;

    /*使能时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);	//使能PORTA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);	//使能CAN1时钟


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
    /*CAN单元设置*/
    CAN_InitStructure.CAN_TTCM = DISABLE;			//非时间触发通信模式
    CAN_InitStructure.CAN_ABOM = DISABLE;			//软件自动离线管理
    CAN_InitStructure.CAN_AWUM = DISABLE;			//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
    CAN_InitStructure.CAN_NART = ENABLE;			//禁止报文自动传送
    CAN_InitStructure.CAN_RFLM = DISABLE;		 	//报文不锁定,新的覆盖旧的
    CAN_InitStructure.CAN_TXFP = DISABLE;			//优先级由报文标识符决定
    CAN_InitStructure.CAN_Mode = 0;				//模式设置： mode:0,普通模式;1,回环模式;

    /*设置波特率*/
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;		//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
    CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq; 		//Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
    CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;		//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
    CAN_InitStructure.CAN_Prescaler = 2;
    CAN_Init(CAN1, &CAN_InitStructure);			//初始化CAN1

    /*设置过滤器*/
    CAN_FilterInitStructure.CAN_FilterNumber = 0;	//过滤器0
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; 	//屏蔽位模式
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; 	//32位宽
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;				//32位ID
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;			//32位MASK
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;	//过滤器0关联到FIFO0
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;				//激活过滤器0
    CAN_FilterInit(&CAN_FilterInitStructure);							//滤波器初始化

    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);	//FIFO0消息挂号中断允许
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // 主优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**
  * @name	USB_LP_CAN1_RX0_IRQHandler()
  * @brief 	Can1接收中断服务函数，每收到一次数据进入一次中断
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
        CAN_Receive(CAN1, CAN_FIFO0, &rx_message);//读取数据
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
