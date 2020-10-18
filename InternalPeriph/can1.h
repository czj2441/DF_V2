#ifndef __CAN1_H
#define __CAN1_H

#include "stm32f10x.h"
#include "dataRecieve.h"

#define can1PortRemap 0//重映射标志位

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
void CAN1_Init(void);
void CAN_Set_Motor_Speed(s16 cm1_iq,s16 cm2_iq,s16 cm3_iq,s16 cm4_iq);

#endif
