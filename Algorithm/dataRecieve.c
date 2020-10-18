#include "dataRecieve.h"

/**
  * @name	_2006_3510_08_dataRecieve()
  * @brief 	CAN1接受中断处理函数,用于接收地盘电机、云台电机、拨弹电机“转子”速度位置反馈
  * @param  Receive:存储电机数据结构体
  * @param  msg:can发送来的数据结构体
  */

void _2006_3510_08_dataRecieve(struct ReceiveTypeDef *Receive, CanRxMsg *msg)
{
	//速度处理
	Receive->speed_real[Receive->speed_count]=(msg->Data[2]<<8)|msg->Data[3];
	Receive->speed_count++;
	if(Receive->speed_count>3)	{
		float temp_sum = Receive->speed_real[0];
		for(int i = 1; i <4; i++)
		{
			temp_sum += Receive->speed_real[i];
		}
		Receive->speed_calc = temp_sum/4;//处理过后的平均数据值
		Receive->speed_count=0;
	}
	
	//位置处理
	Receive->position_real[0]=((msg->Data[0]<<8)|msg->Data[1])/19;//接收到的真实数据值
	if((Receive->position_real[0]-Receive->position_real[1])>40)
	{
		Receive->position_cnt--;
	}
	else if((Receive->position_real[0]-Receive->position_real[1])< -40)
	{
		Receive->position_cnt++;
	}											
	if(Receive->position_cnt > 18)
		Receive->position_cnt = 0;
	else if(Receive->position_cnt < 0)
		Receive->position_cnt = 18;
	
	Receive->position_calc =Receive->position_real[0] + Receive->position_cnt * 82;
	
	Receive->position_real[1]=Receive->position_real[0];
}
