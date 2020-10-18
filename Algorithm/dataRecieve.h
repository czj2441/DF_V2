#ifndef __DATARECIEVE_H
#define __DATARECIEVE_H

#include "stm32f10x.h"
#include "can1.h"

/*can�������ݽṹ��*/
struct ReceiveTypeDef{
	s16 speed_real[8];		//���ܵ����ٶ�����
	u8  speed_count;		//��ǰ���ܵ����ݵĸ���
	s16 speed_calc;			//Ҫ���õ���Чֵ
	s16 now_speed_calc;		//��ǰ���յ��ٶ�
	float k_filler;			//��ͨ�˲�����
	
	s16 position_real[8];	//���յ���λ������
	u8  position_count;		//��ǰ���յ����ݸ���
	s16 position_cnt;		//ת��ת��Ȧ��
	s16 position_calc;		//Ҫ���õ���Чֵ
};

void _2006_3510_08_dataRecieve(struct ReceiveTypeDef *Receive, CanRxMsg *msg);

#endif
