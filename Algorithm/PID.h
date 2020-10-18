#ifndef _PID_H
#define _PID_H

#include "stm32f10x.h"
#include "basicMath.h"

#define SPEED			0
#define POSITION_180	1
#define POSITION_360	2

typedef struct{
    //PID 三参数
    double Kp;			//比例系数，主要控制相应速度，大了容易超调
    double Ki;			//积分系数，用于消除稳态误差，增大小误差范围内的反馈立
    double Kd;			//微分系数，实现具有预测功能，减小超调
    double Ka;			//低通滤波系数 属于[0,1),数越大滤波约明显，当出现明显震荡的时候将此参数调大

    s16 max_out;  		//最大输出
    u16 dead_band;		//PID偏差死区
    u16 intergral_band;	//积分区
    u16 intergral_maxOut;//积分最大输出
    u16 max_input;		//最大输入
    u8 model;			//PID控制类型，0为速度控制，1为位置控制且最大输入对应180度（实际度数有正有负），2为位置控制且最大输入对应360度（实际度数都为正）

    //PID输出值
    s16 output;			//输出
    double P_output;		//P输出
    double I_output;		//I输出
    double D_output;		//D输出

    //误差
    //u16 err_max;			//最大误差
    double err_max;
    double err;				//当前误差
    double err_last;		//上一次的误差
    double d_last;			//上一次的系数D
} PidTypeDef;//int

void PID_Init(PidTypeDef * pid, double kp, double ki, double kd, double ka, double max_out, double dead_band, double i_band, double max_input, double e_max, double i_maxout, u8 model); //PID初始化函数
void PID_Calc(PidTypeDef * pid, double rel_val, double set_val);	//PID计算函数

#endif
