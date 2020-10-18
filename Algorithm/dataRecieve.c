#include "dataRecieve.h"

/**
  * @name	_2006_3510_08_dataRecieve()
  * @brief 	CAN1�����жϴ�����,���ڽ��յ��̵������̨��������������ת�ӡ��ٶ�λ�÷���
  * @param  Receive:�洢������ݽṹ��
  * @param  msg:can�����������ݽṹ��
  */

void _2006_3510_08_dataRecieve(struct ReceiveTypeDef *Receive, CanRxMsg *msg)
{
	//�ٶȴ���
	Receive->speed_real[Receive->speed_count]=(msg->Data[2]<<8)|msg->Data[3];
	Receive->speed_count++;
	if(Receive->speed_count>3)	{
		float temp_sum = Receive->speed_real[0];
		for(int i = 1; i <4; i++)
		{
			temp_sum += Receive->speed_real[i];
		}
		Receive->speed_calc = temp_sum/4;//��������ƽ������ֵ
		Receive->speed_count=0;
	}
	
	//λ�ô���
	Receive->position_real[0]=((msg->Data[0]<<8)|msg->Data[1])/19;//���յ�����ʵ����ֵ
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
