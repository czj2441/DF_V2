#ifndef __DATARECIEVE_H
#define __DATARECIEVE_H

#include "stm32f10x.h"
#include "can1.h"

/*can接收数据结构体*/
struct ReceiveTypeDef{
	s16 speed_real[8];		//接受到的速度数据
	u8  speed_count;		//当前接受的数据的个数
	s16 speed_calc;			//要调用的有效值
	s16 now_speed_calc;		//当前接收的速度
	float k_filler;			//低通滤波参数
	
	s16 position_real[8];	//接收到的位置数据
	u8  position_count;		//当前接收的数据个数
	s16 position_cnt;		//转子转的圈数
	s16 position_calc;		//要调用的有效值
};

void _2006_3510_08_dataRecieve(struct ReceiveTypeDef *Receive, CanRxMsg *msg);

#endif
